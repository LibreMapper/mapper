/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2018 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_OCD_TYPES_V2018_H
#define LIBREMAPPER_OCD_TYPES_V2018_H

#include "ocd_types.h"
#include "ocd_types_v12.h"

namespace Ocd
{
	/*
	 * There is no documentation for version 2018 of the OCD file format.
	 * However, experience shows that the changes happen primarily in the
	 * parameter string contents (EPSG codes, embedded images) plus there is
	 * extra data in OCD like the DEM.
	 */
	struct FormatV2018
	{
		 constexpr static quint16 version = 2018;
		 using FileHeader      = FileHeaderV12;
		 using BaseSymbol      = FormatV11::BaseSymbol;
		 using PointSymbol     = FormatV11::PointSymbol;
		 using LineSymbol      = FormatV11::LineSymbol;
		 using AreaSymbol      = AreaSymbolV12;
		 using TextSymbol      = FormatV11::TextSymbol;
		 using LineTextSymbol  = FormatV11::LineTextSymbol;
		 using RectangleSymbol = FormatV11::RectangleSymbol;
		 using Object          = ObjectV12;
		 using Encoding        = Utf8Encoding;
	};
	
}

OCD_EXPLICIT_INSTANTIATION(extern template, Ocd::FormatV2018)

#endif // LIBREMAPPER_OCD_TYPES_V2018_H
