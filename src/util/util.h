/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012, 2014, 2015, 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_UTIL_H
#define LIBREMAPPER_UTIL_H

#include <cmath>
#include <functional>
#include <type_traits>

#include <QtGlobal>
#include <QtMath>
#include <QPointF>
#include <QRectF>

class QObject;
class QRect;
// IWYU pragma: no_forward_declare QPointF
// IWYU pragma: no_forward_declare QRectF

namespace std
{
	/**
	 * Fallback for missing std::log2 in some distributions of gcc.
	 * 
	 * This template will be selected if std::log2 is not found. The function
	 * std::log2 is part of C++11, but the following distributions of gcc are
	 * known lack this function:
	 * 
	 * - GCC 4.8 in Android NDK R10d
	 * 
	 * The argument must be a floating point value in order to avoid ambiguity
	 * when the regular std::log2 is present.
	 */
	template< class T >
	constexpr double log2(T value)
	{
		static_assert(::std::is_floating_point<T>::value,
					  "The argument to std::log2 must be called a floating point value");
		return log(value)/M_LN2;
	}
}



namespace LibreMapper {

class MapCoord;
class MapCoordF;


// clazy:excludeall=missing-qobject-macro


/** Value to calculate the optimum handle distance of 4 cubic bezier curves
 *  used to approximate a circle. */
#define BEZIER_KAPPA 0.5522847498

/** When drawing a cubic bezier curve, the distance between start and end point
 *  is multiplied by this value to get the handle distance from start respectively
 *  end points.
 * 
 *  Calculated as BEZIER_HANDLE_DISTANCE = BEZIER_KAPPA / sqrt(2) */
#define BEZIER_HANDLE_DISTANCE 0.390524291729



/** (Un-)blocks recursively all signals from a QObject and its child-objects. */
void blockSignalsRecursively(QObject* obj, bool block);

/** Returns a practically "infinitely" big QRectF. */
inline QRectF infiniteRectF()
{
	return QRectF(-10e10, -10e10, 20e10, 20e10);
}

/** Modulus calculation like fmod(x, y), but works correctly for negative x. */
inline double fmod_pos(double x, double y)
{
	return x - y * floor(x / y);
}

/**
 * Enlarges the rect to include the given point.
 * 
 * The given rect must be valid.
 * 
 * \see QRectF::isValid()
 */
void rectInclude(QRectF& rect, const QPointF& point);

/**
 * Enlarges the rect to include the given point.
 * 
 * If the given rect isn't valid, width and height are set to a small positive value.
 */
void rectIncludeSafe(QRectF& rect, const QPointF& point);

/**
 * Enlarges the rect to include the given other_rect.
 * 
 * Both rectangles must be valid.
 * 
 * \see QRectF::isValid()
 */
void rectInclude(QRectF& rect, const QRectF& other_rect);

/**
 * Enlarges the rect to include the given other_rect.
 * 
 * At least one rectangle must be valid.
 */
void rectIncludeSafe(QRectF& rect, const QRectF& other_rect);

/**
 * Enlarges the rect to include the given other_rect.
 * 
 * At least one rectangle must be valid.
 * 
 * \todo Check if QRegion could be used instead.
 */
void rectIncludeSafe(QRect& rect, const QRect& other_rect);


/** Checks for line - rect intersection. */
bool lineIntersectsRect(const QRectF& rect, const QPointF& p1, const QPointF& p2);

/**
 * Calculates the line parameter for a point on a straight line.
 * 
 * Sets ok to false in case of an error (if the line is actually a point,
 * or if the test point is not on the line).
 * 
 * x0, y0: line start
 * dx, dy: vector from line start to line end
 * x, y: suspected point on the line to calculate the parameter for so:
 *       x0 + parameter * dx = x (and the same for y).
 */
double parameterOfPointOnLine(double x0, double y0, double dx, double dy, double x, double y, bool& ok);

/** Checks if the point is on the segment defined by
 *  the given start and end coordinates. */
bool isPointOnSegment(const MapCoordF& seg_start, const MapCoordF& seg_end, const MapCoordF& point);


// TODO: Refactor: put remaining stuff into this namespace, too
namespace Util
{

/**
 * Generates a pattern of parallel lines inside the box given by extent.
 * 
 * @see gridOperation()
 * 
 * @param extent       Extent of the box.
 * @param spacing      Spacing of the lines.
 * @param offset       Offset of the first line from the origin.
 * @param rotation     Angle used to rotate the lines.
 * @param process_line Function object which will be called with start and
 *                     end point for each line.
 */
void hatchingOperation(const QRectF& extent, double spacing, double offset, double rotation,
                       std::function<void (const QPointF&, const QPointF&)>& process_line);

/**
 * Generates a grid of lines inside the given box.
 * 
 * @see hatchingOperation()
 * 
 * @param extent       Extent of the box.
 * @param horz_spacing Horizontal spacing of the lines.
 * @param vert_spacing Vertical spacing of the lines.
 * @param horz_offset  Horizontal offset of the first line from the origin.
 * @param vert_offset  Vertical offset of the first line from the origin.
 * @param rotation     Angle used to rotate the lines.
 * @param process_line Function object which will be called with start and
 *                     end point for each line.
 */
inline
void gridOperation(const QRectF& extent, double horz_spacing, double vert_spacing,
                   double horz_offset, double vert_offset, double rotation,
                   std::function<void (const QPointF&, const QPointF&)>& process_line)
{
	hatchingOperation(extent, horz_spacing, horz_offset, rotation, process_line);
	hatchingOperation(extent, vert_spacing, vert_offset, rotation - M_PI / 2, process_line);
}

/**
 * Tests whether three points form what we would call a corner. The function
 * returns true when point2 lies at least quantum_size from continuation of
 * line from point1 to anchor_point. quantum_size parameter is essential for
 * use of this function in GUI code as for various zoom levels point1 position
 * get quantized with varying step size.
 *
 * @param point1 First point on a line.
 * @param anchor_point Point that is being tested as corner candidate.
 * @param point2 Final point on potentially bent line.
 * @param quantum_size How far can point2 lie from the line defined by
 *        point1--anchor to be considered unaligned with the two other points.
 * @return True if point2 is further than quantum_size from the line defined 
 *         by point1--anchor_point or point2 is on the same side of 
 *         anchor_point as point1.
 */
bool pointsFormCorner(const MapCoord& point1, const MapCoord& anchor_point,
                      const MapCoord& point2, qreal quantum_size);

}  // namespace Util


}  // namespace LibreMapper

#endif
