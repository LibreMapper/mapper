/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_GDAL_FILE_H
#define LIBREMAPPER_GDAL_FILE_H

class QByteArray;

namespace LibreMapper {


/**
 * Utility functions which use GDAL's VSI-aware file API.
 * 
 * Paths must be passed as, and are returned as, UTF-8.
 * 
 * \see https://gdal.org/user/virtual_file_systems.html
 */
namespace GdalFile {

/**
 * Checks if a file exists.
 */
bool exists(const QByteArray& filepath);

/**
 * Checks if a path is regarded as relative.
 */
bool isRelative(const QByteArray& filepath);

/**
 * Checks if a path is an existing directory.
 */
bool isDir(const QByteArray& filepath);

/**
 * Creates a directory.
 */
bool mkdir(const QByteArray& filepath);


/**
 * GDAL-based implementation of Template::tryToFindRelativeTemplateFile().
 * 
 * Returns an absolute path when the given template path identifies an existing
 * file relative to the map path, or an empty value otherwise.
 */
QByteArray tryToFindRelativeTemplateFile(const QByteArray& template_path, const QByteArray& map_path);


}   // namespace GdalUtil

}  // namespace LibreMapper

#endif // LIBREMAPPER_GDAL_FILE_H
