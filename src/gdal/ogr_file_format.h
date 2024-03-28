/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_OGR_FILE_FORMAT_H
#define OPENORIENTEERING_OGR_FILE_FORMAT_H

#include <memory>
#include <vector>

#include <QByteArray>
#include <QString>

#include "fileformats/file_format.h"

namespace OpenOrienteering {

class Importer;
class Exporter;
class Map;
class MapView;


/**
 * A FileFormat for geospatial vector data supported by OGR.
 * 
 * Geospatial vector data cannot be loaded as a regular (OpenOrienteering) Map
 * because it has no scale. However, it typically has a spatial reference, and
 * so it can be imported into an existing map. This is the major reason for
 * implementing the OGR support as a FileFormat.
 */
class OgrFileImportFormat : public FileFormat
{
public:
	/**
	 * Constructs a new OgrFileImportFormat.
	 */
	OgrFileImportFormat();
	
	
	/**
	 * Creates an importer for files supported by OGR.
	 */
	std::unique_ptr<Importer> makeImporter(const QString& path, Map* map, MapView* view) const override;
};

class OgrFileExportFormat : public FileFormat
{
public:
	/**
	  * Constructs a new OgrFileExportFormat.
	  */
	OgrFileExportFormat(QByteArray id, const char* name, const char* extensions);

	/**
	  * Creates an exporter for files supported by OGR.
	  */
	std::unique_ptr<Exporter> makeExporter(const QString &path, const Map *map, const MapView *view) const override;
	
	/**
	 * Returns a container of all supported variants of this format.
	 */
	static std::vector<std::unique_ptr<OgrFileExportFormat>> makeAll();
	
private:
	QByteArray meta_data;
};


}  // namespace OpenOrienteering

#endif // OPENORIENTEERING_OGR_FILE_FORMAT_H
