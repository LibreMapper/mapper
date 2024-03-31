/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2018 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_OCD_ICON_H
#define LIBREMAPPER_OCD_ICON_H


class QImage;


namespace Ocd {

struct IconV8;
struct IconV9;

}  // namespace Ocd 


namespace LibreMapper {

class Map;
class Symbol;


/**
 * A utility for converting OCD icons.
 * 
 * Synopsis:
 * 
 * ocd_base_symbol.icon = OcdIcon{map, symbol};
 * 
 * symbol->setCustomIcon(OcdIcon::toQImage(ocd_base_symbol.icon));
 */
struct OcdIcon
{
	const Map& map;
	const Symbol& symbol;
	
	// Default special member functions are fine.
	
	operator Ocd::IconV8() const;
	operator Ocd::IconV9() const;
	
	/**
	 * Creates a QImage for the given uncompressed icon data.
	 * 
	 * Note: This function does not support OCD version 8 compressed icons.
	 */
	static QImage toQImage(const Ocd::IconV8& icon);
	
	/**
	 * Creates a QImage for the given icon data.
	 */
	static QImage toQImage(const Ocd::IconV9& icon);
};


}  // namespace LibreMapper

#endif
