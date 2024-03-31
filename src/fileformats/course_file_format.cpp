/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2021 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "course_file_format.h"

// IWYU pragma: no_include <algorithm>
#include <memory>

#include <QString>

#include "fileformats/file_import_export.h"
#include "fileformats/iof_course_export.h"
#include "fileformats/kml_course_export.h"


namespace LibreMapper {

template <class Exporter>
std::unique_ptr<CourseFileFormat> makeFileFormat(FileFormat::FileType type, const char* id)
{
	auto builder = [](const QString& path, const Map* map, const MapView* view) {
		return std::make_unique<Exporter>(path, map, view);
	};
	auto const description = Exporter::formatDescription();
	auto const file_extension = Exporter::filenameExtension();
	return std::make_unique<CourseFileFormat>(type, id, description, file_extension, builder);
}


// static
std::vector<std::unique_ptr<CourseFileFormat>> CourseFileFormat::makeAll()
{
	std::vector<std::unique_ptr<CourseFileFormat>> result;
	result.reserve(2);
	
	result.push_back(makeFileFormat<IofCourseExport>(SimpleCourseFile, "simple-iof-course"));
	result.push_back(makeFileFormat<KmlCourseExport>(SimpleCourseFile, "simple-kml-course"));
	return result;
}


CourseFileFormat::CourseFileFormat(FileType type, const char* id, const QString& description, const QString& file_extension, ExporterBuilder exporter_builder)
: FileFormat { type, id, description, file_extension, FileFormat::Feature::FileExport | FileFormat::Feature::WritingLossy }
, make_exporter { exporter_builder }
{
	// Nothing
}


std::unique_ptr<Exporter> CourseFileFormat::makeExporter(const QString& path, const Map* map, const MapView* view) const
{
	return make_exporter(path, map, view);
}


}  // namespace LibreMapper
