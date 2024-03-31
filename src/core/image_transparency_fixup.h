/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_IMAGE_TRANSPARENCY_FIXUP_H
#define LIBREMAPPER_IMAGE_TRANSPARENCY_FIXUP_H

#include <QImage>

namespace LibreMapper {


/**
 * Repairs a pixel composing issue aka QTBUG-100327.
 * 
 * ImageTransparencyFixup repairs a particular issue with composing
 * transparent pixels with Qt5 < 5.15.9 and Qt6 < 6.2.4, tracked
 * upstream as https://bugreports.qt.io/browse/QTBUG-100327, and
 * does nothing otherwise.
 * 
 * QPainter::CompositionMode_Multiply and QPainter::CompositionMode_Darken
 * on a QImage of Format_ARGB32_Premultiplied calculate the resulting alpha
 * channel in a very efficient but not accurate way. A particular case is the
 * composition of two fully transparent pixels which should in theory give a
 * fully transparent pixel. Qt yields an alpha value of 1 (in 0..255) instead.
 * The error accumulates with further compositions.
 * 
 * This class may be used as a functor on a particular image, providing a
 * comfortable way to fix the described case after each composition.
 *
 * Synopsis:
 *
 * QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
 * ImageTransparencyFixup fixup(&image);
 * QPainter p(&image);
 * p.setCompositionMode(QPainter::CompositionMode_Multiply);
 * p.drawImage(another_image);
 * p.end();
 * fixup();
 */
class ImageTransparencyFixup
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 9) || (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && QT_VERSION < QT_VERSION_CHECK(6, 2, 4))
public:
	/**
	 * Create a fixup functor for the given image.
	 * 
	 * The image must be of QImage::Format_ARGB32_Premultiplied.
	 * It may be null.
	 */
	inline explicit ImageTransparencyFixup(QImage* image)
	{
		// NOTE: Here we may add a check for a setting which disables the 
		//       fixup (for better application performance)
		if (image)
		{
			dest = (QRgb*)image->bits();
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
			dest_end = dest + image->sizeInBytes() / sizeof(QRgb);
#else
			dest_end = dest + image->byteCount() / sizeof(QRgb);
#endif
		}
	}
	
	/**
	 * Checks all pixels of the image for the known wrong result of composing
	 * fully transparent pixels, and replaces them with a fully transparent 
	 * pixel.
	 */
	inline void operator()() const
	{
		for (QRgb* px = dest; px != dest_end; px++)
		{
			if (*px == 0x01000000) /* qRgba(0, 0, 0, 1) */
				*px = 0x00000000;  /* qRgba(0, 0, 0, 0) */
		}
	}
	
protected:
	QRgb* dest = nullptr;
	QRgb* dest_end = nullptr;

#else // ^^^ fixup / no-op vvv

public:
	inline explicit ImageTransparencyFixup(QImage*) {}
	inline void operator()() const {}

#endif
};


}  // namespace LibreMapper

#endif
