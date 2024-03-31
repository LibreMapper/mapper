/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2018 Libor Pecháček (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_OCD_GEOREF_FIELDS_H
#define LIBREMAPPER_OCD_GEOREF_FIELDS_H

#include <functional>

class QString;

namespace LibreMapper {

class Georeferencing;


/**
 * OCD type 1039 string field values packed in a struct for convenient
 * handling. Structure element name corresponds to field code and its value
 * is the field value.
 *
 * Fields are initialized with safe default values, commonly found in example
 * files shipped with the program.
 */
struct OcdGeorefFields
{
	double a { 0 };  ///< Real world angle
	int m { 15000 }, ///< Map scale
	    x { 0 },     ///< Real world offset easting
	    y { 0 },     ///< Real world offset northing
	    i { 1000 },  ///< Grid and zone
	    r { 0 };     ///< Real world coord (0 = paper, 1 = real world)

	/**
	 * Fill in provided georef with data extracted from type 1039 string.
	 * @param georef Reference to a Georeferencing object.
	 * @param warning_handler Function to handle import warnings.
	 */
	void setupGeoref(Georeferencing& georef,
	                 const std::function<void (const QString&)>& warning_handler) const;

	/**
	 * Translate from Mapper CRS representation into OCD one.
	 * @param georef Source Georeferencing.
	 * @param warning_handler Function to handle conversion warnings.
	 * @return Data for OCD type 1039 string compilation.
	 */
	static OcdGeorefFields fromGeoref(const Georeferencing& georef,
	                                  const std::function<void (const QString&)>& warning_handler);
};

/**
 * Equal-to operator comparing this structure with another. Field `a` gets
 * compared with 8-digit precision.
 */
bool operator==(const OcdGeorefFields& lhs, const OcdGeorefFields& rhs);

/**
 * Trivial non-equal-to operator.
 * @see operator==(const OcdGeorefFields&, const OcdGeorefFields&)
 */
inline
bool operator!=(const OcdGeorefFields& lhs, const OcdGeorefFields& rhs)
{
	return !operator==(lhs, rhs);
}

}  // namespace LibreMapper

#endif // LIBREMAPPER_OCD_GEOREF_FIELDS_H
