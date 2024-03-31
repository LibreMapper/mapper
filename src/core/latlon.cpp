/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013, 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "latlon.h"

#include <QDebug>


namespace LibreMapper {

QDebug operator<<(QDebug dbg, const LatLon& lat_lon)
{
	dbg.space() 
	  << "LatLon"
	  << lat_lon.latitude() << lat_lon.longitude();
	return dbg.space();
}


}  // namespace LibreMapper
