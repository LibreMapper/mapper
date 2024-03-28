/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_FIRFILTER_H
#define COVE_FIRFILTER_H

#include <vector>

#include <QImage>
#include <QRgb>

namespace cove {
class ProgressObserver;

class FIRFilter
{
protected:
	std::vector<std::vector<double>> matrix;

public:
	FIRFilter(unsigned radius = 0);
	FIRFilter& binomic();
	FIRFilter& box();
	FIRFilter& a(double center);
	QImage apply(const QImage& source,
	             QRgb outOfBoundsColor = qRgb(128, 128, 128),
	             ProgressObserver* progressObserver = nullptr);
};
} // cove

#endif
