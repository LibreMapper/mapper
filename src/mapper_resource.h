/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_MAPPER_RESOURCE_H
#define OPENORIENTEERING_MAPPER_RESOURCE_H

namespace OpenOrienteering {


namespace MapperResource {

/**
 * Initializes QDir::searchPaths for Mapper resource prefixes.
 * 
 * This function registers the prefixes "data" and "doc" with paths leading
 * to possible locations in the build dir (development build only), application
 * bundle (Android, macOS, Windows), system (Linux) and Qt resource system.
 * Thus resource lookup may either iterate explicitly over
 * QDir::searchPaths("PREFIX"), or directly access resources using 
 * QFile("PREFIX:name").
 */
void setSeachPaths();

}  // namespace MapperResource


}  // namespace OpenOrienteering


#endif
