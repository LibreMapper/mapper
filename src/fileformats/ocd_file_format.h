/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013, 2014, 2016-2018 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_OCD_FILE_FORMAT_H
#define LIBREMAPPER_OCD_FILE_FORMAT_H

#include <memory>
#include <vector>

#include <QtGlobal>
#include <QString>

#include "fileformats/file_format.h"

namespace LibreMapper {

class Exporter;
class Importer;
class Map;
class MapView;


/**
 * The map file format known as OC*D.
 */
class OcdFileFormat : public FileFormat
{
public:
	/**
	 * Returns the file format ID string for the given version.
	 */
	static const char* idForVersion(quint16 version);
	
	/**
	 * Returns a container of all supported variants of this format.
	 */
	static std::vector<std::unique_ptr<OcdFileFormat>> makeAll();
	
	
	/**
	 * Constructs a new OcdFileFormat.
	 * 
	 * Supported explicit OCD versions for export are 8 to 12.
	 * In addition to these versions, the following special values are valid:
	 * - `autoDeterminedVersion()` supports import only. It determines the
	 *   version from the imported data.
	 * 
	 * Supplying an unsupported version will cause the program to abort.
	 */
	OcdFileFormat(quint16 version);
	
	
	/**
	 * Detects whether the buffer may be the start of a valid OCD file.
	 * 
	 * At the moment, it requires at least two bytes of data.
	 * It will return false if compiled for a big endian system.
	 */
	ImportSupportAssumption understands(const char* buffer, int size) const override;
	
	
	/// \copydoc FileFormat::createImporter()
	std::unique_ptr<Importer> makeImporter(const QString& path, Map* map, MapView* view) const override;
	
	/// \copydoc FileFormat::createExporter()
	std::unique_ptr<Exporter> makeExporter(const QString& path, const Map* map, const MapView* view) const override;
	
	/// The name of the property where the importer can record the imported version.
	static constexpr const char* versionProperty() { return "OcdFileFormat::version"; }
	
	/// A special value which indicates the usage of an auto-detected (or default) version.
	static constexpr quint16 autoDeterminedVersion() { return 0; }
	
private:
	quint16 version = 0;
	
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_OCD_FILE_FORMAT_H
