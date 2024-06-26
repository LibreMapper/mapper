/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Pete Curtis (OpenOrienteering)
 * Copyright 2018-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "file_format.h"

#include <algorithm>
#include <iterator>

#include <Qt>
#include <QCoreApplication>
#include <QFileInfo>
#include <QLatin1Char>
#include <QLatin1String>

#include "mapper_config.h"
#include "fileformats/file_import_export.h"


namespace LibreMapper {

// ### FileFormatException ###

// virtual
FileFormatException::~FileFormatException() noexcept = default;

// virtual
const char* FileFormatException::what() const noexcept
{
	return msg_c.constData();
}


// static
FileFormatException FileFormatException::internalError(const char* function_info)
{
	return FileFormatException {
		QCoreApplication::translate("LibreMapper::Util",
		                            "Internal error detected!"
		                            " Please report this issue."
		                            "\nVersion: %1"
		                            "\nLocation: %2")
		        .arg(QString::fromUtf8(APP_VERSION),
		             QString::fromUtf8(function_info))
	};
}



// ### FileFormat ###

FileFormat::FileFormat(FileFormat::FileType file_type, const char* id, const QString& description, const QString& file_extension, Features features)
 : file_type(file_type),
   format_id(id),
   format_description(description),
   format_features(features)
{
	Q_ASSERT(file_type != 0);
	Q_ASSERT(qstrlen(id) > 0);
	Q_ASSERT(!description.isEmpty());
	if (!file_extension.isEmpty())
		addExtension(file_extension);
}

FileFormat::~FileFormat() = default;


void FileFormat::addExtension(const QString& file_extension)
{
	file_extensions << file_extension;
	format_filter.clear();
}


bool FileFormat::supportsReading() const
{
	return supportsFileOpen() || supportsFileImport();
}

bool FileFormat::supportsWriting() const
{
	return supportsFileSave() || supportsFileSaveAs() || supportsFileExport();
}


FileFormat::ImportSupportAssumption FileFormat::understands(const char* /*buffer*/, int /*size*/) const
{
	return supportsReading() ? Unknown : NotSupported;
}


QString FileFormat::fixupExtension(QString filepath) const
{
	auto const& extensions = fileExtensions();
	if (!extensions.empty())
	{
		using std::begin; using std::end;
		auto const has_extension = std::any_of(begin(extensions), end(extensions), [&filepath](const auto& extension) {
			return filepath.endsWith(extension, Qt::CaseInsensitive)
			        && QStringView{filepath}.mid(filepath.length() - extension.length() - 1, 1) == QLatin1String(".");
		});
		if (!has_extension)
		{
			if (!filepath.endsWith(QLatin1Char('.')))
				filepath.append(QLatin1Char('.'));
			filepath.append(primaryExtension());
		}
		
		// Ensure that the file name matches the format.
		Q_ASSERT(extensions.contains(QFileInfo(filepath).suffix()));
	}
	return filepath;
}


std::unique_ptr<Importer> FileFormat::makeImporter(const QString& /*path*/, Map* /*map*/, MapView* /*view*/) const
{
	qWarning("Format '%s' does not support import", format_id);
	return nullptr;
}

std::unique_ptr<Exporter> FileFormat::makeExporter(const QString& /*path*/, const Map* /*map*/, const MapView* /*view*/) const
{
	qWarning("Format '%s' does not support export", format_id);
	return nullptr;
}

const QString& LibreMapper::FileFormat::filter() const
{
	if (format_filter.isEmpty())
	{
		auto const label = [](QString description) {
			description.replace(QLatin1Char('('), QLatin1Char('['));
			description.replace(QLatin1Char(')'), QLatin1Char(']'));
			return description;
		} (format_description);
		auto const extensions = file_extensions.join(QStringLiteral(" *."));
		format_filter = label + QLatin1String(" (*.") + extensions + QLatin1String(")");
	}
	return format_filter;
}


}  // namespace LibreMapper
