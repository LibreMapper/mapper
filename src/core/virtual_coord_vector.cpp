/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "virtual_coord_vector.h"

#include "core/map_coord.h"


namespace OpenOrienteering {

//### VirtualCoordVector ###

MapCoordF VirtualCoordVector::fromMapCoordF(size_type index) const
{
	return (*coords)[index];
}

MapCoordF VirtualCoordVector::fromMapCoord(size_type index) const
{
	return MapCoordF(flags[index]);
}


}  // namespace OpenOrienteering
