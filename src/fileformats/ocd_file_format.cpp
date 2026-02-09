/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013-2016, 2018 Kai Pastor (OpenOrienteering)
 * Some parts taken from file_format_oc*d8{.h,_p.h,cpp} which are
 * Copyright 2012 Pete Curtis (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "ocd_file_format.h"

// IWYU pragma: no_include <algorithm>
#include <memory>

#include <QtGlobal>
#include <QFlags>
#include <QString>

#include "fileformats/file_import_export.h"
#include "fileformats/ocd_file_export.h"
#include "fileformats/ocd_file_import.h"


namespace LibreMapper {

namespace {

QString labelForVersion(quint16 version)
{
	switch (version)
	{
	case OcdFileFormat::autoDeterminedVersion():
		return ::LibreMapper::ImportExport::tr("OCAD");
	default:
		return ::LibreMapper::ImportExport::tr("OCAD version %1").arg(version);
	}
}

FileFormat::Features featuresForVersion(quint16 version)
{
	switch (version)
	{
	case OcdFileFormat::autoDeterminedVersion():
		// Intentionally no FileFormat::ExportSupported. This prevents this
		// format from being shown in the Save-as dialog. However, it is legal
		// to create an exporter for the autoDeterminedVersion(). The actual
		// export version will be determined from the Map's versionProperty()
		// if possible, or from OcdFileExport::default_version.
		return FileFormat::Feature::FileOpen | FileFormat::Feature::FileImport | FileFormat::Feature::ReadingLossy |
		       FileFormat::Feature::FileSave | FileFormat::Feature::WritingLossy;
		
	default:
		// Intentionally no FileFormat::ImportSupported. Import is handled
		// by the autoDeterminedVersion().
		return FileFormat::Feature::FileSave | FileFormat::Feature::FileSaveAs | FileFormat::Feature::WritingLossy;
	}
}

}  // namespace



// ### OcdFileFormat ###

// static
const char* OcdFileFormat::idForVersion(quint16 version)
{
	switch (version)
	{
	case OcdFileFormat::autoDeterminedVersion():
		return "OCD";
	case 8:
		return "OCD8";
	case 9:
		return "OCD9";
	case 10:
		return "OCD10";
	case 11:
		return "OCD11";
	case 12:
		return "OCD12";
	case 2018:
		return "OCD2018";
	default:
		qFatal("Unsupported OCD version");
	}
}


// static
std::vector<std::unique_ptr<OcdFileFormat>> OcdFileFormat::makeAll()
{
	std::vector<std::unique_ptr<OcdFileFormat>> result;
	result.reserve(7);
	result.push_back(std::make_unique<OcdFileFormat>(autoDeterminedVersion()));
	result.push_back(std::make_unique<OcdFileFormat>(2018));
	result.push_back(std::make_unique<OcdFileFormat>(12));
	result.push_back(std::make_unique<OcdFileFormat>(11));
	result.push_back(std::make_unique<OcdFileFormat>(10));
	result.push_back(std::make_unique<OcdFileFormat>(9));
	result.push_back(std::make_unique<OcdFileFormat>(8));
	return result;
}



OcdFileFormat::OcdFileFormat(quint16 version)
: FileFormat { MapFile, idForVersion(version), labelForVersion(version), QStringLiteral("ocd"), featuresForVersion(version) }
, version { version }
{
	// Nothing
}



FileFormat::ImportSupportAssumption OcdFileFormat::understands(const char* buffer, int size) const
{
	// The first two bytes of the file must be AD 0C.
	if (size < 2)
		return Unknown;
	else if (quint8(buffer[0]) == 0xAD && buffer[1] == 0x0C)
		return FullySupported;
	else
		return NotSupported;
}


std::unique_ptr<Importer> OcdFileFormat::makeImporter(const QString& path, Map *map, MapView *view) const
{
	return std::make_unique<OcdFileImport>(path, map, view);
}

std::unique_ptr<Exporter> OcdFileFormat::makeExporter(const QString& path, const Map* map, const MapView* view) const
{
	return std::make_unique<OcdFileExport>(path, map, view, version);
}


}  // namespace LibreMapper
