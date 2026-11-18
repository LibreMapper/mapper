/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2018 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_IMAGE_DIFFERENCE_H
#define LIBREMAPPER_IMAGE_DIFFERENCE_H

#include <QtGlobal>

class QImage;
class QPoint;


namespace LibreMapper {

/**
 * Tests if any pixel in the image region given by x0, y0, x1, y1
 * approximately matches the expected value.
 */
bool fuzzyComparePixel(const QImage& image, int x0, int y0, int x1, int y1, const quint32 expected);

/**
 * Tests if the images are different, ignoring edges which differ in location by one pixel.
 *
 * Returns { -1, -1 } when the images are equal,
 * the minimum width and height when the images have different sizes,
 * or the coordinates of the first pixel which does not match (fuzzily).
 * In the second case, it uses QCOMPARE to output the sizes.
 * In the last case, it uses QCOMPARE to output the pixel values.
 */
QPoint fuzzyDifference(const QImage& lhs, const QImage& rhs);

}  // namespace LibreMapper

#endif  // LIBREMAPPER_IMAGE_DIFFERENCE_H
