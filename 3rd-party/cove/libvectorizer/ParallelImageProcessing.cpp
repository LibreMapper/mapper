/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 * Copyright 2020 Kai Pastor
 *
 * This file is part of CoVe.
 */

#include "ParallelImageProcessing.h"

#include <algorithm>

#include <QtGlobal>
#include <QImage>
#include <QRgb>


namespace cove {

// ### HorizontalStripes ###

QImage HorizontalStripes::makeStripe(const QImage& original, int scanline, int stripe_height)
{
	auto without_color_table = [](const QImage& original, int scanline, int stripe_height) -> QImage {
		return QImage(original.constScanLine(scanline),
		              original.width(),
		              std::min(scanline + stripe_height, original.height()) - scanline,
		              original.bytesPerLine(),
		              original.format());
	};
	auto with_color_table = [](const QImage& original, int scanline, int stripe_height) -> QImage {
		// Cf. https://bugreports.qt.io/browse/QTBUG-81674
		// Setting the color table would cause detaching if we use the
		// const char* taking constructor, so we const_cast<char*> to select
		// the other constructor.
		// But now we need a way to enforce detaching on non-const access to the
		// returned image. We achieve this by creating a shared copy of the
		// returned image carried in the cleanup data, and thus bound to its
		// lifetime.
		auto* cleanup_data = new QImage();
		auto cleanup_function = [](void* data) { delete static_cast<QImage*>(data); };
		QImage stripe(const_cast<uchar*>(original.constScanLine(scanline)),
		              original.width(),
		              std::min(scanline + stripe_height, original.height()) - scanline,
		              original.bytesPerLine(),
		              original.format(),
		              cleanup_function,
		              cleanup_data);
		stripe.setColorTable(original.colorTable());
		*cleanup_data = stripe;  // increase reference count -> force detach on write.
		return stripe;
	};
	
	auto const delegate = original.colorCount() == 0 ? without_color_table : with_color_table;
	return delegate(original, scanline, stripe_height);
}

QImage HorizontalStripes::makeStripe(QImage& original, int scanline, int stripe_height)
{
	QImage result(original.scanLine(scanline),
	              original.width(),
	              std::min(scanline + stripe_height, original.height()) - scanline,
	              original.bytesPerLine(),
	              original.format() );
	if (original.colorCount() > 0)
		result.setColorTable(original.colorTable());
	return result;
}


}  // namespace cove

