/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "template.h"

#include <algorithm>
#include <cmath>
#include <iosfwd>
#include <iterator>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <Qt>
#include <QtMath>
#include <QByteArray>
#include <QChar>
#include <QColor>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLatin1Char>
#include <QLatin1String>
#include <QMessageBox>
#include <QPainter>
#include <QRectF>
#include <QSizeF>
#include <QStringView>
#include <QTransform>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "core/georeferencing.h"
#include "core/map_view.h"
#include "core/map.h"
#include "core/objects/object.h"
#include "fileformats/file_format.h"
#include "gdal/gdal_template.h"
#include "gdal/ogr_template.h"
#include "gui/file_dialog.h"
#include "templates/template_image.h"
#include "templates/template_map.h"
#include "templates/template_placeholder.h"
#include "templates/template_track.h"
#include "util/util.h"
#include "util/xml_stream_util.h"


namespace LibreMapper {

class Template::ScopedOffsetReversal
{
public:
	explicit ScopedOffsetReversal(Template& temp)
	: temp(temp)
	, copy(temp.transform)
	, needed(temp.accounted_offset != MapCoord{} )
	{
		if (needed)
		{
			// Temporarily revert the offset the saved data is expected to
			// match the original data before bounds checking / correction
			temp.applyTemplatePositionOffset();
		}
	}
	
	~ScopedOffsetReversal()
	{
		if (needed)
		{
			temp.transform = copy;
			temp.updateTransformationMatrices();
		}
	}
	
	ScopedOffsetReversal(const ScopedOffsetReversal&) = delete;
	ScopedOffsetReversal(ScopedOffsetReversal&&) = delete;
	ScopedOffsetReversal& operator=(const ScopedOffsetReversal&) = delete;
	ScopedOffsetReversal&& operator=(ScopedOffsetReversal&&) = delete;
	
private:
	Template& temp;
	const TemplateTransform copy;
	const bool needed;
};



// ### TemplateTransform ###

Q_STATIC_ASSERT(std::is_standard_layout<TemplateTransform>::value);
Q_STATIC_ASSERT(std::is_nothrow_default_constructible<TemplateTransform>::value);
Q_STATIC_ASSERT(std::is_nothrow_copy_constructible<TemplateTransform>::value);
Q_STATIC_ASSERT(std::is_nothrow_move_constructible<TemplateTransform>::value);
Q_STATIC_ASSERT(std::is_nothrow_copy_assignable<TemplateTransform>::value);
Q_STATIC_ASSERT(std::is_nothrow_move_assignable<TemplateTransform>::value);

// static
TemplateTransform TemplateTransform::fromQTransform(const QTransform& qt) noexcept
{
	const auto rotation = std::atan2(qt.m21()-qt.m12(), qt.m11()+qt.m22());

	// Determine the non-rotating, pure scaling aspect.
	// Note TemplateTransform rotation is opposite to QTransform.
	// This rotation makes the shear components equal.
	QTransform scaling = qt * (qFuzzyIsNull(rotation) ? QTransform() : QTransform().rotateRadians(rotation));
	return { qRound(1000 * qt.m31()),
	         qRound(1000 * qt.m32()),
	         rotation,
	         scaling.m11(),
	         scaling.m22(),
	         (scaling.m12()+scaling.m21())/2 };
}


struct NonShearingObjectTransform
{
	TemplateTransform const& transform;
	
	void operator()(Object* object) const
	{
		object->scale(transform.template_scale_x, transform.template_scale_y);
		object->rotate(transform.template_rotation);
		object->move(transform.template_x, transform.template_y);
	}
};

struct ShearingObjectTransform
{
	TemplateTransform const& transform;
	QTransform const scaling;
	
	void operator()(Object* object) const
	{
		object->transform(scaling);
		object->rotate(transform.template_rotation);
		object->move(transform.template_x, transform.template_y);
	}
};

std::function<void (Object*)> TemplateTransform::makeObjectTransform() const
{
	if (qFuzzyIsNull(template_shear))
		return NonShearingObjectTransform{*this};
	
	return ShearingObjectTransform{*this,
	                               {template_scale_x, template_shear,
	                                template_shear, template_scale_y,
	                                0, 0}};
}


void TemplateTransform::save(QXmlStreamWriter& xml, const QString& role) const
{
	xml.writeStartElement(QString::fromLatin1("transformation"));
	xml.writeAttribute(QString::fromLatin1("role"), role);
	xml.writeAttribute(QString::fromLatin1("x"), QString::number(template_x));
	xml.writeAttribute(QString::fromLatin1("y"), QString::number(template_y));
	xml.writeAttribute(QString::fromLatin1("scale_x"), QString::number(template_scale_x));
	xml.writeAttribute(QString::fromLatin1("scale_y"), QString::number(template_scale_y));
	if (!qFuzzyIsNull(template_shear))
		xml.writeAttribute(QString::fromLatin1("shear"), QString::number(template_shear));
	xml.writeAttribute(QString::fromLatin1("rotation"), QString::number(template_rotation));
	xml.writeEndElement(/*transformation*/);
}

void TemplateTransform::load(QXmlStreamReader& xml)
{
	Q_ASSERT(xml.name() == QLatin1String("transformation"));
	
	XmlElementReader element { xml };
	auto x64 = element.attribute<qint64>(QLatin1String("x"));
	auto y64 = element.attribute<qint64>(QLatin1String("y"));
	auto coord = MapCoord::fromNative64withOffset(x64, y64);
	template_x = coord.nativeX();
	template_y = coord.nativeY();
	template_scale_x = element.attribute<double>(QLatin1String("scale_x"));
	template_scale_y = element.attribute<double>(QLatin1String("scale_y"));
	template_shear = element.attribute<double>(QLatin1String("shear")); // 0 if absent
	template_rotation = element.attribute<double>(QLatin1String("rotation"));
}

bool operator==(const TemplateTransform& lhs, const TemplateTransform& rhs) noexcept
{
	return lhs.template_x == rhs.template_x
	        && lhs.template_y == rhs.template_y
	        && (qFuzzyCompare(lhs.template_rotation, rhs.template_rotation)
	            || (qIsNull(lhs.template_rotation) && qIsNull(rhs.template_rotation)))
	        && qFuzzyCompare(lhs.template_scale_x, rhs.template_scale_x)
	        && qFuzzyCompare(lhs.template_scale_y, rhs.template_scale_y)
	        && (qFuzzyCompare(lhs.template_shear, rhs.template_shear)
	            || (qFuzzyIsNull(lhs.template_shear) && qFuzzyIsNull(rhs.template_shear)));
}

bool operator!=(const TemplateTransform& lhs, const TemplateTransform& rhs) noexcept
{
	return !operator==(lhs, rhs);
}



// ### Template ###

bool Template::suppressAbsolutePaths = false;


Template::Template(const QString& path, not_null<Map*> map)
: map(map)
{
	QFileInfo file_info{path};
	template_file = file_info.fileName();
	
	auto canonical_path = file_info.canonicalFilePath();
	template_path = canonical_path.isEmpty() ? path : canonical_path;
	
	updateTransformationMatrices();
}

Template::Template(const Template& proto)
: QObject(nullptr)
, map(proto.map)
, template_file(proto.template_file)
, template_path(proto.template_path)
, template_relative_path(proto.template_relative_path)
, template_state(proto.template_state)
, error_string(proto.error_string)
, has_unsaved_changes(proto.has_unsaved_changes)
, is_georeferenced(proto.is_georeferenced)
, accounted_offset(proto.accounted_offset)
, transform(proto.transform)
, other_transform(proto.other_transform)
, adjusted(proto.adjusted)
, adjustment_dirty(proto.adjustment_dirty)
, passpoints(proto.passpoints)
, template_group(proto.template_group)
, map_to_template(proto.map_to_template)
, template_to_map(proto.template_to_map)
, template_to_map_other(proto.template_to_map_other)
{
	// nothing else
}

Template::~Template()
{
	Q_ASSERT(template_state != Loaded);
}

QString Template::errorString() const
{
	return error_string;
}

void Template::setErrorString(const QString &text)
{
	error_string = text;
}



void Template::saveTemplateConfiguration(QXmlStreamWriter& xml, bool open, const QDir* map_dir) const
{
	xml.writeStartElement(QString::fromLatin1("template"));
	xml.writeAttribute(QString::fromLatin1("type"), QString::fromUtf8(getTemplateType()));
	xml.writeAttribute(QString::fromLatin1("open"), QString::fromLatin1(open ? "true" : "false"));
	xml.writeAttribute(QString::fromLatin1("name"), getTemplateFilename());
	auto primary_path = getTemplatePath();
	auto relative_path = getTemplateRelativePath(map_dir);
	if (suppressAbsolutePaths && QFileInfo(primary_path).isAbsolute())
		primary_path = relative_path;
	xml.writeAttribute(QString::fromLatin1("path"), primary_path);
	xml.writeAttribute(QString::fromLatin1("relpath"), relative_path);
	
	if (template_group)
	{
		xml.writeAttribute(QString::fromLatin1("group"), QString::number(template_group));
	}
	
	if (is_georeferenced)
	{
		xml.writeAttribute(QString::fromLatin1("georef"), QString::fromLatin1("true"));
	}
	else
	{
		ScopedOffsetReversal no_offset{*const_cast<Template*>(this)};
		
		xml.writeStartElement(QString::fromLatin1("transformations"));
		if (adjusted)
			xml.writeAttribute(QString::fromLatin1("adjusted"), QString::fromLatin1("true"));
		if (adjustment_dirty)
			xml.writeAttribute(QString::fromLatin1("adjustment_dirty"), QString::fromLatin1("true"));
		int num_passpoints = (int)passpoints.size();
		xml.writeAttribute(QString::fromLatin1("passpoints"), QString::number(num_passpoints));
		
		transform.save(xml, QString::fromLatin1("active"));
		other_transform.save(xml, QString::fromLatin1("other"));
		
		for (int i = 0; i < num_passpoints; ++i)
			passpoints[i].save(xml);
		
		map_to_template.save(xml, QString::fromLatin1("map_to_template"));
		template_to_map.save(xml, QString::fromLatin1("template_to_map"));
		template_to_map_other.save(xml, QString::fromLatin1("template_to_map_other"));
		
		xml.writeEndElement(/*transformations*/);
	}
	
	saveTypeSpecificTemplateConfiguration(xml);
	
	xml.writeEndElement(/*template*/);
}

std::unique_ptr<Template> Template::loadTemplateConfiguration(QXmlStreamReader& xml, Map& map, bool& open)
{
	Q_ASSERT(xml.name() == QLatin1String("template"));
	
	QXmlStreamAttributes attributes = xml.attributes();
	if (attributes.hasAttribute(QLatin1String("open")))
		open = (attributes.value(QLatin1String("open")) == QLatin1String("true"));
	
	auto const type = attributes.value(QLatin1String("type"));
	QString path = attributes.value(QLatin1String("path")).toString();
	auto temp = templateForType(type, path, &map);
	if (!temp && type.length() == 0)
		temp = templateForPath(path, &map);
	if (!temp)
		temp = std::make_unique<TemplatePlaceholder>(type.toUtf8(), path, &map);
	
	temp->setTemplateRelativePath(attributes.value(QLatin1String("relpath")).toString());
	if (attributes.hasAttribute(QLatin1String("name")))
		temp->template_file = attributes.value(QLatin1String("name")).toString();
	temp->is_georeferenced = (attributes.value(QLatin1String("georef")) == QLatin1String("true"));
	if (attributes.hasAttribute(QLatin1String("group")))
		temp->template_group = attributes.value(QLatin1String("group")).toInt();
		
	while (xml.readNextStartElement())
	{
		if (!temp->is_georeferenced && xml.name() == QLatin1String("transformations"))
		{
			temp->adjusted = (xml.attributes().value(QLatin1String("adjusted")) == QLatin1String("true"));
			temp->adjustment_dirty = (xml.attributes().value(QLatin1String("adjustment_dirty")) == QLatin1String("true"));
			int num_passpoints = xml.attributes().value(QLatin1String("passpoints")).toInt();
Q_ASSERT(temp->passpoints.size() == 0);
			temp->passpoints.reserve(qMin(num_passpoints, 10)); // 10 is not a limit
			
			bool other_transform_loaded = false;
			while (xml.readNextStartElement())
			{
				QStringView role = xml.attributes().value(QLatin1String("role"));
				if (xml.name() == QLatin1String("transformation"))
				{
					if (role == QLatin1String("active"))
					{
						temp->transform.load(xml);
					}
					else if (xml.attributes().value(QLatin1String("role")) == QLatin1String("other"))
					{
						try {
							temp->other_transform.load(xml);
							other_transform_loaded = true;
						}
						catch (const std::range_error& e)
						{
							// Assuming that ensureBoundsForQint32 failed. Cf. map_coord.cpp and GH-1969.
							qDebug("Unable to load %s", xml.qualifiedName().toLocal8Bit().constData());
						}
					}
					else
					{
						qDebug("%s", xml.qualifiedName().toLocal8Bit().constData());
						xml.skipCurrentElement(); // unsupported
					}
				}
				else if (xml.name() == QLatin1String("passpoint"))
				{
					temp->passpoints.push_back(PassPoint::load(xml));
				}
				else if (xml.name() == QLatin1String("matrix"))
				{
					if (role == QLatin1String("map_to_template"))
						temp->map_to_template.load(xml);
					else if (role == QLatin1String("template_to_map"))
						temp->template_to_map.load(xml);
					else if (role == QLatin1String("template_to_map_other"))
						temp->template_to_map_other.load(xml);
					else
					{
						qDebug("%s", xml.qualifiedName().toLocal8Bit().constData());
						xml.skipCurrentElement(); // unsupported
					}
				}
				else
				{
					qDebug("%s", xml.qualifiedName().toLocal8Bit().constData());
					xml.skipCurrentElement(); // unsupported
				}
			}
			if (!other_transform_loaded)
			{
				temp->other_transform = temp->transform;
			}
		}
		else if (!temp->loadTypeSpecificTemplateConfiguration(xml))
		{
			temp.reset();
			break;
		}
	}
	
	if (temp && !temp->is_georeferenced)
	{
		// Fix template adjustment after moving objects during import (cf. #513)
		const auto offset = MapCoord::boundsOffset();
		if (!offset.isZero())
		{
			if (temp->template_to_map.getCols() == 3 && temp->template_to_map.getRows() == 3)
			{
				temp->template_to_map.set(0, 2, temp->template_to_map.get(0, 2) - offset.x / 1000.0);
				temp->template_to_map.set(1, 2, temp->template_to_map.get(1, 2) - offset.y / 1000.0);
				temp->template_to_map.invert(temp->map_to_template);
			}
				
			if (temp->template_to_map_other.getCols() == 3 && temp->template_to_map_other.getRows() == 3)
			{
				temp->template_to_map_other.set(0, 2, temp->template_to_map_other.get(0, 2) - offset.x / 1000.0);
				temp->template_to_map_other.set(1, 2, temp->template_to_map_other.get(1, 2) - offset.y / 1000.0);
			}
		}
		
		// Fix template alignment problems caused by grivation rounding since version 0.6
		const double correction = map.getGeoreferencing().getGrivationError();
		if (qAbs(correction) != 0.0
		    && (qstrcmp(temp->getTemplateType(), "TemplateTrack") == 0
		        || qstrcmp(temp->getTemplateType(), "OgrTemplate") == 0) )
		{
			temp->setTemplateRotation(temp->getTemplateRotation() + Georeferencing::degToRad(correction));
		}
	}
	
	if (temp && !temp->finishTypeSpecificTemplateConfiguration())
		temp.reset();
	
	if (temp)
		temp->setTemplateState(Unloaded);
	
	return temp;
}

bool Template::saveTemplateFile() const
{
	return false;
}

void Template::switchTemplateFile(const QString& new_path, bool load_file)
{
	if (template_state == Loaded)
	{
		setTemplateAreaDirty();
		unloadTemplateFile();
	}
	
	template_path          = new_path;
	template_file          = QFileInfo(new_path).fileName();
	template_relative_path = QString();
	template_state         = Template::Unloaded;
	
	if (load_file)
		loadTemplateFile();
}

bool Template::execSwitchTemplateFileDialog(QWidget* dialog_parent)
{
	QString new_path = FileDialog::getOpenFileName(dialog_parent,
	                                               tr("Find the moved template file"),
	                                               QString(),
	                                               tr("All files (*.*)") );
	new_path = QFileInfo(new_path).canonicalFilePath();
	if (new_path.isEmpty())
		return false;
	
	const State   old_state = getTemplateState();
	const QString old_path  = getTemplatePath();
	
	if (auto* placeholder = qobject_cast<TemplatePlaceholder*>(this))
	{
		setTemplatePath(new_path);
		auto new_temp = placeholder->makeActualTemplate();
		if (new_temp)
		{
			auto pos = map->findTemplateIndex(this);
			if (pos >= 0)
			{
				auto self = map->setTemplate(pos, std::move(new_temp));
				if (map->getTemplate(pos)->loadTemplateFile())
				{
					// Loading succeeded. This object must be destroyed.
					self.release()->deleteLater();
					return true;
				}
				// Loading failed. This object must be put back.
				map->setTemplate(pos, std::move(self));
				return false;
			}
		}
	}
	else
	{
		switchTemplateFile(new_path, true);
	}
	if (getTemplateState() != Loaded)
	{
		QString error_template = QCoreApplication::translate("LibreMapper::TemplateListWidget", "Cannot open template\n%1:\n%2").arg(new_path);
		QString error = errorString();
		Q_ASSERT(!error.isEmpty());
		
		// Revert change
		switchTemplateFile(old_path, old_state == Loaded);
		if (old_state == Invalid)
		{
			template_state = Invalid;
		}
		
		QMessageBox::warning(dialog_parent, tr("Error"), error_template.arg(error));
		return false;
	}
	
	return true;
}

bool Template::setupAndLoad(QWidget* dialog_parent, const MapView* view)
{
	Q_ASSERT(getTemplateState() == Template::Configuring);
	
	bool center_in_view = true;
	
	if (!preLoadSetup(dialog_parent))
		return false;
	if (!loadTemplateFile())
		return false;
	if (!postLoadSetup(dialog_parent, center_in_view))
	{
		unloadTemplateFile();
		return false;
	}
	
	// If the template is not georeferenced, position it at the viewport midpoint
	if (!isTemplateGeoreferenced() && center_in_view)
	{
		auto offset = MapCoord { calculateTemplateBoundingBox().center() };
		setTemplatePosition(view->center() - offset + templatePosition());
	}
	
	setTemplateState(Loaded);
	setTemplateAreaDirty();
	emit templateStateChanged();
	return true;
}



Template::LookupResult Template::tryToFindTemplateFile(const QString& map_path)
{
	// This function normally sets the state either to Invalid or Unloaded.
	// However, the Loaded state must not be changed here because this would
	// cause inconsistencies with other data held by templates in this state.
	auto const set_state = [this](auto proposed_state) {
		if (template_state != Loaded)
			template_state = proposed_state;
	};
	
	// Determining the directory from a file or directory
	auto const dir = [](const QString& path) -> QDir {
		auto const path_info = QFileInfo(path);
		if (path_info.isFile())
			return path_info.dir();
		if (path_info.isDir())
			return { path };
		return {};
	};
	
	// 1. The relative path with regard to the map directory, if both are valid
	auto const rel_path = getTemplateRelativePath();
	if (!rel_path.isEmpty() && !map_path.isEmpty())
	{
		auto const abs_path_info = QFileInfo(dir(map_path).absoluteFilePath(rel_path));
		if (abs_path_info.isFile())
		{
			setTemplateFileInfo(abs_path_info);
			set_state(Unloaded);
			return FoundByRelPath;
		}
	}
	
	// 2. The absolute path of the template
	auto const template_path_info = QFileInfo(getTemplatePath());
	if (template_path_info.isFile())
	{
		/* setTemplateFileInfo(template_path_info); */
		set_state(Unloaded);
		return FoundByAbsPath;
	}
	
	// 3. The map directory, if valid, for the filename of the template
	auto const filename = getTemplateFilename();
	if (!filename.isEmpty() && !map_path.isEmpty())
	{
		auto const abs_path_info = QFileInfo(dir(map_path).absoluteFilePath(filename));
		if (abs_path_info.isFile())
		{
			setTemplateFileInfo(abs_path_info);
			set_state(Unloaded);
			return FoundInMapDir;
		}
	}
	
	set_state(Invalid);
	setErrorString(tr("No such file."));
	return NotFound;
}

bool Template::fileExists() const
{
	return QFileInfo::exists(template_path);
}


bool Template::tryToFindAndReloadTemplateFile(const QString& map_path)
{
	return tryToFindTemplateFile(map_path) != NotFound
	       && loadTemplateFile();
}

bool Template::preLoadSetup(QWidget* /*dialog_parent*/)
{
	return true;
}

bool Template::loadTemplateFile()
{
	Q_ASSERT(template_state != Loaded);
	
	const State old_state = template_state;
	
	setErrorString(QString());
	try
	{
		if (!fileExists())
		{
			template_state = Invalid;
			setErrorString(tr("No such file."));
		}
		else if (!loadTemplateFileImpl())
		{
			template_state = Invalid;
			if (errorString().isEmpty())
			{
				qDebug("%s: Missing error message from failure in %s::loadTemplateFileImpl(bool)", \
				         Q_FUNC_INFO, \
				         getTemplateType() );
				setErrorString(tr("Is the format of the file correct for this template type?"));
			}
		}
		else if (old_state != Configuring)
		{
			template_state = Loaded;
			setTemplateAreaDirty();
		}
	}
	catch (std::bad_alloc&)
	{
		template_state = Invalid;
		setErrorString(tr("Not enough free memory."));
	}
	catch (FileFormatException& e)
	{
		template_state = Invalid;
		setErrorString(e.message());
	}
	
	if (old_state != template_state)
		emit templateStateChanged();
		
	return template_state != Invalid;
}

bool Template::postLoadSetup(QWidget* /*dialog_parent*/, bool& /*out_center_in_view*/)
{
	return true;
}

void Template::unloadTemplateFile()
{
	Q_ASSERT(template_state == Loaded || template_state == Configuring);
	if (hasUnsavedChanges())
	{
		// The changes are lost
		setHasUnsavedChanges(false);
	}
	unloadTemplateFileImpl();
	template_state = Unloaded;
	emit templateStateChanged();
}


// virtual
bool Template::canChangeTemplateGeoreferenced() const
{
	return false;
}

// virtual
bool Template::trySetTemplateGeoreferenced(bool value, QWidget* /*dialog_parent*/)
{
	return isTemplateGeoreferenced() == value;
}


void Template::applyTemplateTransform(QPainter* painter) const
{
	painter->translate(transform.template_x / 1000.0, transform.template_y / 1000.0);
	// Rotate counter-clockwise.
	painter->rotate(-transform.template_rotation * (180 / M_PI));

	// Scale
	if (qFuzzyIsNull(transform.template_shear))
	{
		painter->scale(transform.template_scale_x, transform.template_scale_y);
	}
	else
	{
		QTransform scaling(transform.template_scale_x, transform.template_shear,
		                   transform.template_shear, transform.template_scale_y,
		                   0, 0);
		painter->setTransform(scaling, true);
	}
}

QRectF Template::getTemplateExtent() const
{
	Q_ASSERT(!is_georeferenced);
	return infiniteRectF();
}

QRectF Template::boundingRect() const
{
	auto const extent = getTemplateExtent();
	auto rect = QRectF { templateToMap(extent.topLeft()), QSizeF{} };
	rectInclude(rect, templateToMap(extent.topRight()));
	rectInclude(rect, templateToMap(extent.bottomRight()));
	rectInclude(rect, templateToMap(extent.bottomLeft()));
	return rect;
}

void Template::scale(double factor, const MapCoord& center)
{
	Q_ASSERT(!is_georeferenced);
	setTemplatePosition(center + factor * (templatePosition() - center));
	setTemplateScaleX(factor * getTemplateScaleX());
	setTemplateScaleY(factor * getTemplateScaleY());
	
	accounted_offset *= factor;
}

void Template::rotate(double rotation, const MapCoord& center)
{
	Q_ASSERT(!is_georeferenced);
	
	auto offset = templatePositionOffset();
	setTemplatePositionOffset({});
	
	setTemplateRotation(getTemplateRotation() + rotation);
	
	auto position = MapCoordF{templatePosition() - center};
	position.rotate(-rotation);
	setTemplatePosition(MapCoord{position} + center);
	
	setTemplatePositionOffset(offset);
}

void Template::setTemplateAreaDirty()
{
	QRectF template_area = calculateTemplateBoundingBox();
	map->setTemplateAreaDirty(this, template_area, getTemplateBoundingBoxPixelBorder());	// TODO: Would be better to do this with the corner points, instead of the bounding box
}

bool Template::canBeDrawnOnto() const
{
	return false;
}

QRectF Template::calculateTemplateBoundingBox() const
{
	// Create bounding box by calculating the positions of all corners of the transformed extent rect
	QRectF extent = getTemplateExtent();
	QRectF bbox;
	rectIncludeSafe(bbox, templateToMap(extent.topLeft()));
	rectInclude(bbox, templateToMap(extent.topRight()));
	rectInclude(bbox, templateToMap(extent.bottomRight()));
	rectInclude(bbox, templateToMap(extent.bottomLeft()));
	return bbox;
}

void Template::drawOntoTemplate(not_null<MapCoordF*> coords, int num_coords, const QColor& color, qreal width, QRectF map_bbox, ScribbleOptions mode)
{
	Q_ASSERT(canBeDrawnOnto());
	Q_ASSERT(num_coords > 1);
	
	if (!map_bbox.isValid())
	{
		map_bbox = QRectF(coords[0].x(), coords[0].y(), 0, 0);
		for (int i = 1; i < num_coords; ++i)
			rectInclude(map_bbox, coords[i]);
	}
	auto const radius = qMin(getTemplateScaleX(), getTemplateScaleY()) * qMax((width+1) / 2, 1.0);
	QRectF radius_bbox = QRectF(map_bbox.left() - radius, map_bbox.top() - radius,
								map_bbox.width() + 2*radius, map_bbox.height() + 2*radius);
	
	drawOntoTemplateImpl(coords, num_coords, color, width, mode);
	map->setTemplateAreaDirty(this, radius_bbox, 0);
	
	setHasUnsavedChanges(true);
}

void Template::drawOntoTemplateUndo(bool /*redo*/)
{
	// nothing
}

void Template::addPassPoint(const PassPoint& point, int pos)
{
	Q_ASSERT(!is_georeferenced);
	passpoints.insert(passpoints.begin() + pos, point);
}
void Template::deletePassPoint(int pos)
{
	passpoints.erase(passpoints.begin() + pos);
}
void Template::clearPassPoints()
{
	passpoints.clear();
	setAdjustmentDirty(true);
	adjusted = false;
}

void Template::switchTransforms()
{
	Q_ASSERT(!is_georeferenced);
	setTemplateAreaDirty();
	
	TemplateTransform temp = transform;
	transform = other_transform;
	other_transform = temp;
	template_to_map_other = template_to_map;
	updateTransformationMatrices();
	
	adjusted = !adjusted;
	setTemplateAreaDirty();
	map->emitTemplateChanged(this);
}
void Template::getTransform(TemplateTransform& out) const
{
	Q_ASSERT(!is_georeferenced);
	out = transform;
}
void Template::setTransform(const TemplateTransform& transform)
{
	Q_ASSERT(!is_georeferenced);
	setTemplateAreaDirty();
	
	this->transform = transform;
	updateTransformationMatrices();
	
	setTemplateAreaDirty();
	map->emitTemplateChanged(this);
}
void Template::getOtherTransform(TemplateTransform& out) const
{
	Q_ASSERT(!is_georeferenced);
	out = other_transform;
}
void Template::setOtherTransform(const TemplateTransform& transform)
{
	Q_ASSERT(!is_georeferenced);
	other_transform = transform;
}

void Template::setTemplateFileInfo(const QFileInfo& file_info)
{
	template_path = file_info.canonicalFilePath();
	if (template_path.isEmpty())
		template_path = file_info.filePath();
	template_file = file_info.fileName();
}

void Template::setTemplatePath(const QString& value)
{
	setTemplateFileInfo(QFileInfo(value));
}

QString Template::getTemplateRelativePath(const QDir* map_dir) const
{
	auto path = getTemplateRelativePath();
	if (getTemplateState() != Invalid && map_dir)
		path = map_dir->relativeFilePath(getTemplatePath());
	if (path.isEmpty())
		path = getTemplateFilename();
	return path;
}

void Template::setHasUnsavedChanges(bool value)
{
	has_unsaved_changes = value;
	if (value)
		map->setTemplatesDirty();
}

void Template::setAdjustmentDirty(bool value)
{
	adjustment_dirty = value;
	if (value)
		map->setTemplatesDirty();
}



bool Template::hasAlpha() const
{
	return template_state == Template::Loaded;
}



const std::vector<QByteArray>& Template::supportedExtensions()
{
	static std::vector<QByteArray> extensions;
	if (extensions.empty())
	{
		auto& image_extensions = TemplateImage::supportedExtensions();
		auto& map_extensions   = TemplateMap::supportedExtensions();
#ifdef MAPPER_USE_GDAL
		auto& gdal_extensions  = GdalTemplate::supportedExtensions();
		auto& ogr_extensions   = OgrTemplate::supportedExtensions();
#else
		auto const gdal_extensions = std::vector<QByteArray>{ };
		auto const& ogr_extensions = gdal_extensions;
#endif
		auto& track_extensions = TemplateTrack::supportedExtensions();
		extensions.reserve(image_extensions.size()
		                   + map_extensions.size()
		                   + gdal_extensions.size()
		                   + ogr_extensions.size()
		                   + track_extensions.size());
		extensions.insert(end(extensions), begin(image_extensions), end(image_extensions));
		extensions.insert(end(extensions), begin(map_extensions), end(map_extensions));
		extensions.insert(end(extensions), begin(gdal_extensions), end(gdal_extensions));
		extensions.insert(end(extensions), begin(ogr_extensions), end(ogr_extensions));
		extensions.insert(end(extensions), begin(track_extensions), end(track_extensions));
	}
	return extensions;
}

namespace {

bool endsWithAnyOf(const QString& path, const std::vector<QByteArray>& list)
{
	using namespace std;
	return any_of(begin(list), end(list), [path](const QByteArray& extension) {
		return path.size() > extension.size()
		       && path[path.size() - extension.size() - 1] == QLatin1Char('.')
		       && path.endsWith(QLatin1String(extension), Qt::CaseInsensitive);
	} );
}

}  // namespace

std::unique_ptr<Template> Template::templateForPath(const QString& path, Map* map)
{
#ifdef MAPPER_USE_GDAL
#  define HANDLED_BY_GDAL(expr) expr
#else
#  define HANDLED_BY_GDAL(expr) false
#endif
	
	std::unique_ptr<Template> t;
	if (endsWithAnyOf(path, TemplateImage::supportedExtensions())
	    && !HANDLED_BY_GDAL(endsWithAnyOf(path, GdalTemplate::supportedExtensions())))
		t = std::make_unique<TemplateImage>(path, map);
	else if (endsWithAnyOf(path, TemplateMap::supportedExtensions()))
		t = std::make_unique<TemplateMap>(path, map);
	else if (endsWithAnyOf(path, TemplateTrack::supportedExtensions())
	         && !HANDLED_BY_GDAL(endsWithAnyOf(path, OgrTemplate::supportedExtensions())))
		t = std::make_unique<TemplateTrack>(path, map);
#ifdef MAPPER_USE_GDAL
	else if (GdalTemplate::canRead(path))
		t = std::make_unique<GdalTemplate>(path, map);
	else if (OgrTemplate::canRead(path))
		t = std::make_unique<OgrTemplate>(path, map);
	else if (endsWithAnyOf(path, GdalTemplate::supportedExtensions())
	         && !endsWithAnyOf(path, OgrTemplate::supportedExtensions()))
		t = std::make_unique<GdalTemplate>(path, map);
	else if (endsWithAnyOf(path, OgrTemplate::supportedExtensions())
	         && !endsWithAnyOf(path, GdalTemplate::supportedExtensions()))
		t = std::make_unique<OgrTemplate>(path, map);
#endif
	
	return t;
}

std::unique_ptr<Template> Template::templateForType(const QStringView type, const QString& path, Map* map)
{
	std::unique_ptr<Template> t;
	
	// We must respect the customizable handling of tracks,
	// which is reflected in OgrTemplate::supportedExtensions().
	auto const is_track = endsWithAnyOf(path, TemplateTrack::supportedExtensions());
#ifdef MAPPER_USE_GDAL
	auto const track_with_gdal = (is_track && endsWithAnyOf(path, OgrTemplate::supportedExtensions()));
#else
	constexpr auto const track_with_gdal = false;
#endif
	
	auto type_cstring = type.toUtf8();
	if (type_cstring == "TemplateImage")
		t = std::make_unique<TemplateImage>(path, map);
	else if (type_cstring == "TemplateMap")
		t = std::make_unique<TemplateMap>(path, map);
	else if (type_cstring == "OgrTemplate" && is_track && !track_with_gdal)
		t = std::make_unique<TemplateTrack>(path, map);
	else if (type_cstring == "TemplateTrack" && !track_with_gdal)
		t = std::make_unique<TemplateTrack>(path, map);
#ifdef MAPPER_USE_GDAL
	else if (type_cstring == "GdalTemplate")
		t = std::make_unique<GdalTemplate>(path, map);
	else if (type_cstring == "OgrTemplate" || type_cstring == "TemplateTrack")
		t = std::make_unique<OgrTemplate>(path, map);
#endif
	return t;
}



void Template::saveTypeSpecificTemplateConfiguration(QXmlStreamWriter& /*xml*/) const
{
	// nothing
}

bool Template::loadTypeSpecificTemplateConfiguration(QXmlStreamReader& xml)
{
	xml.skipCurrentElement();
	return true;
}

bool Template::finishTypeSpecificTemplateConfiguration()
{
	return true;
}



void Template::drawOntoTemplateImpl(MapCoordF* /*coords*/, int /*num_coords*/, const QColor& /*color*/, qreal /*width*/, ScribbleOptions /*mode*/)
{
	// nothing
}

MapCoord Template::templatePosition() const
{
	return MapCoord::fromNative(transform.template_x, transform.template_y);
}


void Template::setTemplatePosition(const MapCoord& coord)
{
	transform.template_x = coord.nativeX();
	transform.template_y = coord.nativeY();
	updateTransformationMatrices();
}

MapCoord Template::templatePositionOffset() const
{
	return accounted_offset;
}

void Template::setTemplatePositionOffset(const MapCoord& offset)
{
	const auto move = accounted_offset - offset;
	if (move != MapCoord{})
	{
		accounted_offset = move;
		applyTemplatePositionOffset();
	}
	accounted_offset = offset;
}

void Template::applyTemplatePositionOffset()
{
	QTransform t;
	t.rotate(-qRadiansToDegrees(getTemplateRotation()));
	const double shear = getTemplateShear();
	QTransform scaling(getTemplateScaleX(), shear,
	                   shear, getTemplateScaleY(),
	                   0, 0);
	setTemplatePosition(templatePosition() - MapCoord{(scaling*t).map(QPointF{accounted_offset})});
}

void Template::resetTemplatePositionOffset()
{
	accounted_offset = {};
}


void Template::updateTransformationMatrices()
{
	double shear = getTemplateShear();
	double cosr = cos(-transform.template_rotation);
	double sinr = sin(-transform.template_rotation);
	double scale_x = getTemplateScaleX();
	double scale_y = getTemplateScaleY();
	
	template_to_map.setSize(3, 3);
	template_to_map.set(0, 0, scale_x * cosr -   shear * sinr);
	template_to_map.set(0, 1,   shear * cosr - scale_y * sinr);
	template_to_map.set(1, 0, scale_x * sinr +   shear * cosr);
	template_to_map.set(1, 1,   shear * sinr + scale_y * cosr);
	template_to_map.set(0, 2, transform.template_x / 1000.0);
	template_to_map.set(1, 2, transform.template_y / 1000.0);
	template_to_map.set(2, 0, 0);
	template_to_map.set(2, 1, 0);
	template_to_map.set(2, 2, 1);
	
	template_to_map.invert(map_to_template);
}


}  // namespace LibreMapper
