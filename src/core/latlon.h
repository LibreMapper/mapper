/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013, 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_LATLON_H
#define OPENORIENTEERING_LATLON_H

#include <QtMath>

class QDebug;

namespace OpenOrienteering {

/**
 * @brief Specifies geographic location by latitude and longitude.
 * 
 * Latitude is a geographic coordinate that specifies the north-south
 * position (φ, phi).
 * 
 * Longitude is a geographic coordinate that specifies the east-west 
 * position (λ, lambda).
 * 
 * LatLon is meant to be similar to QGeoCoordinate (part of Qt since 5.2).
 * This Qt class might eventually replace LatLon. QGeoCoordinate has altitude
 * as an additional property which is rarely used in Mapper at the moment.
 * 
 * @see QGeoCoordinate (https://doc.qt.io/qt-5/qgeocoordinate.html)
 */
class LatLon
{
private:
	double latitude_value;
	double longitude_value;
	friend QDebug operator<<(QDebug dbg, const LatLon& lat_lon);
	
public:
	/**
	 * Constructs a new LatLon for the latitude and longitude given in degrees.
	 */
	constexpr LatLon(double latitude = 0.0, double longitude = 0.0) noexcept;
	
	/**
	 * Returns a new LatLon for the latitude and longitude given in radiant.
	 */
	constexpr static LatLon fromRadiant(double latitude, double longitude) noexcept;
	
	/**
	 * Returns the latitude value in degrees.
	 */
	constexpr double latitude() const;
	
	/**
	 * Returns the longitude value in degrees.
	 */
	constexpr double longitude() const;
	
	/**
	 * Returns true if this object has exactly the same latitude and longitude
	 * like another. FP precision issues are not taken into account.
	 */
	constexpr bool operator==(const LatLon& rhs) const;
	
	/**
	 * Returns true if this object has not exactly the same latitude and longitude
	 * like another. FP precision issues are not taken into account.
	 */
	constexpr bool operator!=(const LatLon& rhs) const;
};

/**
 * Dumps a LatLon to the debug output.
 * 
 * Note that this requires a *reference*, not a pointer.
 */
QDebug operator<<(QDebug dbg, const LatLon &lat_lon);



//### LatLon inline code ###

inline
constexpr LatLon::LatLon(double latitude, double longitude) noexcept
: latitude_value(latitude)
, longitude_value(longitude)
{
	; // nothing
}

inline
constexpr LatLon LatLon::fromRadiant(double latitude, double longitude) noexcept
{
	return LatLon(qRadiansToDegrees(latitude), qRadiansToDegrees(longitude));
}

inline
constexpr double LatLon::latitude() const
{
	return latitude_value;
}

inline
constexpr double LatLon::longitude() const
{
	return longitude_value;
}

inline
constexpr bool LatLon::operator==(const LatLon& rhs) const
{
	return (this->latitude_value == rhs.latitude_value) && (this->longitude_value == rhs.longitude_value);
}

inline
constexpr bool LatLon::operator!=(const LatLon& rhs) const
{
	return !(*this == rhs);
}


}  // namespace OpenOrienteering

#endif
