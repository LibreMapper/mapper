/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013, 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "global.h"

#include "mapper_config.h" // IWYU pragma: keep

#include "fileformats/course_file_format.h"
#include "fileformats/file_format_registry.h"
#include "fileformats/xml_file_format.h"
#include "fileformats/ocd_file_format.h"
#include "gdal/ogr_file_format.h"


namespace LibreMapper {

void doStaticInitializations()
{
	// Register the supported file formats
	FileFormats.registerFormat(new XMLFileFormat());
#ifndef MAPPER_BIG_ENDIAN
	for (auto&& format : OcdFileFormat::makeAll())
		FileFormats.registerFormat(format.release());
#endif
#ifdef MAPPER_USE_GDAL
	FileFormats.registerFormat(new OgrFileImportFormat());
	for (auto&& format : OgrFileExportFormat::makeAll())
		FileFormats.registerFormat(format.release());
#endif
	for (auto&& format : CourseFileFormat::makeAll())
		FileFormats.registerFormat(format.release());
}


}  // namespace LibreMapper
