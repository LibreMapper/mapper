/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2021 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_COURSE_FILE_FORMAT_H
#define OPENORIENTEERING_COURSE_FILE_FORMAT_H

#include <functional>
#include <memory>
#include <vector>

#include "fileformats/file_format.h"

class QString;


namespace OpenOrienteering {

class Exporter;
class Map;
class MapView;

/**
 * A family of formats representing courses.
 */
class CourseFileFormat : public FileFormat
{
public:
	using ExporterBuilder = std::function<std::unique_ptr<Exporter> (const QString&, const Map*, const MapView*)>;
	
	/**
	 * Returns a container of all supported variants of this format.
	 */
	static std::vector<std::unique_ptr<CourseFileFormat>> makeAll();
	
	
	/**
	 * Constructs a new CourseFileFormat.
	 */
	CourseFileFormat(FileType type, const char* id, const QString& description, const QString& file_extension, ExporterBuilder exporter_builder);
	
	
	/// \copydoc FileFormat::makeExporter()
	std::unique_ptr<Exporter> makeExporter(const QString& path, const Map* map, const MapView* view) const override;
	
private:
	ExporterBuilder make_exporter;
	
};


}  // namespace OpenOrienteering

#endif // OPENORIENTEERING_COURSE_FILE_FORMAT_H
