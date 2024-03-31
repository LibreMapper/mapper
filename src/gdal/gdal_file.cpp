/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "gdal_file.h"

#include <cpl_conv.h>
#include <cpl_vsi.h>
#include <gdal.h>

#include <QByteArray>
#include <QString>


namespace LibreMapper {

namespace GdalFile {

bool exists(const QByteArray& filepath)
{
	VSIStatBufL stat_buf;
	return VSIStatExL(filepath, &stat_buf, VSI_STAT_EXISTS_FLAG) == 0;
}

bool isRelative(const QByteArray& filepath)
{
	return CPLIsFilenameRelative(filepath) == TRUE;
}


bool isDir(const QByteArray& filepath)
{
	VSIStatBufL stat_buf;
	return VSIStatExL(filepath, &stat_buf, VSI_STAT_EXISTS_FLAG | VSI_STAT_NATURE_FLAG) == 0
	       && VSI_ISDIR(stat_buf.st_mode);
}

bool mkdir(const QByteArray& filepath)
{
	// 0777 would be right for POSIX mkdir with umask, but we
	// cannot rely on umask for all paths supported by VSIMkdir.
	return VSIMkdir(filepath, 0755) == 0;
}


QByteArray tryToFindRelativeTemplateFile(const QByteArray& template_path, const QByteArray& map_path)
{
	QByteArray filepath = map_path + '/' + template_path;
	if (!exists(filepath))
		filepath = QByteArray(CPLGetDirname(map_path)) + '/' + template_path;
	if (!exists(filepath))
		filepath.clear();
	return filepath;
}

}   // namespace GdalUtil

}  // namespace LibreMapper
