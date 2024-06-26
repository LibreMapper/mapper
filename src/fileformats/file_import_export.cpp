/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Pete Curtis (OpenOrienteering)
 * Copyright 2013, 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "file_import_export.h"

#include <exception>
#include <memory>
#include <utility>

#include <QtGlobal>
#include <QtMath>
#include <QChar>
#include <QFile>
#include <QFileInfo>  // IWYU pragma: keep
#include <QFlags>
#include <QIODevice>
#include <QLatin1Char>
#include <QLineF>
#include <QObject>
#include <QSaveFile>
#include <QScopedValueRollback>

#include "core/georeferencing.h"
#include "core/map.h"
#include "core/map_part.h"
#include "core/map_view.h"
#include "core/storage_location.h"  // IWYU pragma: keep
#include "core/objects/object.h"
#include "core/symbols/line_symbol.h"
#include "core/symbols/point_symbol.h"
#include "core/symbols/symbol.h"
#include "fileformats/file_format.h"
#include "templates/template.h"
#include "templates/template_placeholder.h"


namespace LibreMapper {

// ### ImportExport ###

ImportExport::~ImportExport() = default;


bool ImportExport::supportsQIODevice() const noexcept
{
	return true;
}

void ImportExport::setDevice(QIODevice* device)
{
	device_ = device;
}


QVariant ImportExport::option(const QString& name) const
{
	if (!options.contains(name))
		throw FileFormatException(::LibreMapper::ImportExport::tr("No such option: %1", "No such import / export option").arg(name));
	return options[name];
}


void ImportExport::setOption(const QString& name, const QVariant& value)
{
	options[name] = value;
}


// ### Importer ###

Importer::~Importer() = default;


void Importer::setLoadSymbolsOnly(bool value)
{
	load_symbols_only = value;
}


bool Importer::doImport()
{
	std::unique_ptr<QFile> managed_file;
	QScopedValueRollback<QIODevice*> original_device{device_};
	if (supportsQIODevice())
	{
		if (!device_)
		{
			managed_file = std::make_unique<QFile>(path);
			device_ = managed_file.get();
		}
		if (!device_->isOpen() && !device_->open(QIODevice::ReadOnly))
		{
			addWarning(tr("Cannot open file\n%1:\n%2").arg(path, device_->errorString()));
			return false;
		}
	}
	
	try
	{
		prepare();
		if (!importImplementation())
		{
			Q_ASSERT(!warnings().empty());
			importFailed();
			return false;
		}
		validate();
	}
	catch (std::exception &e)
	{
		importFailed();
		addWarning(tr("Cannot open file\n%1:\n%2").arg(path, QString::fromLocal8Bit(e.what())));
		return false;
	}
	
	return true;
}


void Importer::prepare()
{}

// Don't add warnings in this function. They may hide the error message.
void Importer::importFailed()
{}

void Importer::validate()
{
	auto const& georef = map->getGeoreferencing();
	if (georef.getState() == Georeferencing::Geospatial)
	{
		auto const expected = georef.getProjectedRefPoint();
		auto const actual = georef.toProjectedCoords(georef.getGeographicRefPoint());
		auto const offset = QLineF(actual, expected).length();
		if (offset > 0.9)
		{
			addWarning(::LibreMapper::Importer::tr("Georeferencing mismatch:") + QChar::Space +
			           ::LibreMapper::Importer::tr("Data may appear shifted by %1 m.").arg(qCeil(offset)) + QChar::Space +
			           ::LibreMapper::Importer::tr("Check the reference point coordinates in the georeferencing dialog."));
		}
	}
	
	// Object post processing:
	// - make sure that there is no object without symbol
	// - make sure that all area-only path objects are closed
	// - make sure that there are no special points in wrong places (e.g. curve starts inside curves)
	for (int p = 0; p < map->getNumParts(); ++p)
	{
		MapPart* part = map->getPart(p);
		for (int o = 0; o < part->getNumObjects(); ++o)
		{
			Object* object = part->getObject(o);
			if (object->getSymbol() == nullptr)
			{
				addWarning(::LibreMapper::Importer::tr("Found an object without symbol."));
				if (object->getType() == Object::Point)
					object->setSymbol(map->getUndefinedPoint(), true);
				else if (object->getType() == Object::Path)
					object->setSymbol(map->getUndefinedLine(), true);
				else
				{
					// There is no undefined symbol for this type of object, delete the object
					part->deleteObject(o);
					--o;
					continue;
				}
			}
			
			if (object->getType() == Object::Path)
			{
				PathObject* path = object->asPath();
				auto contained_types = path->getSymbol()->getContainedTypes();
				if (contained_types & Symbol::Area && !(contained_types & Symbol::Line))
					path->closeAllParts();
				
				path->normalize();
			}
		}
	}
	
	if (auto deleted = map->deleteIrregularObjects())
	{
		addWarning(tr("Dropped %n irregular object(s).", nullptr, int(deleted)));
	}
	
	// Symbol post processing
	for (int i = 0; i < map->getNumSymbols(); ++i)
	{
		if (!map->getSymbol(i)->loadingFinishedEvent(map))
			throw FileFormatException(::LibreMapper::Importer::tr("Error during symbol post-processing."));
	}
	
	// Template post processing
	bool have_lost_template = false;
	for (int i = 0; i < map->getNumTemplates(); ++i)
	{
		QString error_string;
		Template* temp = map->getTemplate(i);
		
		// Resolve actual filepath.
		switch (temp->tryToFindTemplateFile(path))
		{
		case Template::NotFound:
			have_lost_template = true;
			continue;
		case Template::FoundInMapDir:
			error_string.append(
			            ::LibreMapper::Importer::tr(
			                "Template \"%1\" has been loaded from the map's directory instead of"
			                " the relative location to the map file where it was previously.")
			            .arg(temp->getTemplateFilename()) + QLatin1Char('\n') );
		default:
			;
		}
		
		// Migrate templates of undefined types, based on the now-resolved path.
		if (auto* placeholder = qobject_cast<TemplatePlaceholder*>(temp))
		{
			if (auto actual = placeholder->makeActualTemplate())
			{
				temp = actual.get();
				map->setTemplate(i, std::move(actual));
				if (view)
					view->setTemplateVisibility(temp, view->getTemplateVisibility(placeholder));
			}
		}
		
		// Report warnings
		error_string.append(temp->errorString());
		if (!error_string.isEmpty())
		{
			addWarning(tr("Warnings when loading template '%1':\n%2")
			           .arg(temp->getTemplateFilename(), error_string));
		}
	}
	
	if (have_lost_template)
	{
#if defined(Q_OS_ANDROID)
		addWarning(tr("At least one template file could not be found."));
#else
		addWarning(tr("At least one template file could not be found.") + QLatin1Char(' ') +
		           tr("Click the red template name(s) in the Templates -> Template setup window to locate the template file name(s)."));
#endif
	}
	
	if (view)
	{
		view->setPanOffset({0,0});
	}

	// Update all objects without trying to remove their renderables first, this gives a significant speedup when loading large files
	map->updateAllObjects(); // TODO: is the comment above still applicable?
}



// ### Exporter ###

Exporter::~Exporter() = default;


bool Exporter::doExport()
{
	std::unique_ptr<QSaveFile> managed_file;
	QScopedValueRollback<QIODevice*> original_device{device_};
	if (supportsQIODevice())
	{
		if (!device_)
		{
			managed_file = std::make_unique<QSaveFile>(path);
			device_ = managed_file.get();
		}
		if (!device_->isOpen() && !device_->open(QIODevice::WriteOnly))
		{
			addWarning(device_->errorString());
			return false;
		}
	}
	
	auto success = true;
	// Save the map
	try
	{
		if (!exportImplementation())
		{
			Q_ASSERT(!warnings().empty());
			success = false;
		}
		if (success && managed_file && !managed_file->commit())
		{
			addWarning(managed_file->errorString());
			success = false;
		}
#ifdef Q_OS_ANDROID
		// Make the MediaScanner aware of the *updated* file.
		auto* file_device = qobject_cast<QFileDevice*>(device_);
		if (success && file_device)
		{
			const auto file_info = QFileInfo(file_device->fileName());
			Android::mediaScannerScanFile(file_info.absolutePath());
		}
#endif
	}
	catch (std::exception &e)
	{
		addWarning(QString::fromLocal8Bit(e.what()));
		success = false;
	}
	
	// Save modified templates
	for (auto i = 0; i < map->getNumTemplates(); ++i)
	{
		auto const* temp = map->getTemplate(i);
		auto const filename = temp->getTemplateFilename();
		try
		{
			if (temp->hasUnsavedChanges() && !temp->saveTemplateFile())
			{
				addWarning(tr("Cannot save file\n%1:\n%2").arg(filename, temp->errorString()));
				success = false;
			}
		}
		catch (std::exception &e)
		{
			addWarning(tr("Cannot save file\n%1:\n%2").arg(filename, QString::fromLocal8Bit(e.what())));
			success = false;
		}
	}
	
	return success;
}


}  // namespace LibreMapper
