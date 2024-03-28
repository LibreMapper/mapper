/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "qpainter_t.h"

#include <Qt>
#include <QtTest>
#include <QRgb>

#include "core/image_transparency_fixup.h"

using namespace OpenOrienteering;


QPainterTest::QPainterTest(QObject* parent)
: QObject(parent),
  white_img(makeImage(Qt::white)),
  black_img(makeImage(Qt::black)),
  trans_img(makeImage(Qt::transparent))
{
	// nothing
}

void QPainterTest::initTestCase()
{
	QCOMPARE(white_img.pixel(0,0), qRgba(255, 255, 255, 255));
	QCOMPARE(trans_img.pixel(0,0), qRgba(0, 0, 0, 0));
	QCOMPARE(black_img.pixel(0,0), qRgba(0, 0, 0, 255));
}

void QPainterTest::sourceOverCompostion()
{
	const QPainter::CompositionMode source_over = QPainter::CompositionMode_SourceOver;
	
	QCOMPARE(compose(white_img, black_img, source_over).pixel(0,0), qRgba(255, 255, 255, 255));
	QCOMPARE(compose(black_img, white_img, source_over).pixel(0,0), qRgba(0, 0, 0, 255));
	
	QCOMPARE(compose(white_img, trans_img, source_over).pixel(0,0), qRgba(255, 255, 255, 255));
	QCOMPARE(compose(trans_img, white_img, source_over).pixel(0,0), qRgba(255, 255, 255, 255));
	
	QCOMPARE(compose(black_img, trans_img, source_over).pixel(0,0), qRgba(0, 0, 0, 255));
	QCOMPARE(compose(trans_img, black_img, source_over).pixel(0,0), qRgba(0, 0, 0, 255));
	
	QCOMPARE(compose(white_img, white_img, source_over).pixel(0,0), qRgba(255, 255, 255, 255));
	QCOMPARE(compose(black_img, black_img, source_over).pixel(0,0), qRgba(0, 0, 0, 255));
	
	QCOMPARE(compose(trans_img, trans_img, source_over).pixel(0,0), qRgba(0, 0, 0, 0));
}

void QPainterTest::multiplyComposition()
{
	const QPainter::CompositionMode multiply = QPainter::CompositionMode_Multiply;
	
	QCOMPARE(compose(white_img, black_img, multiply).pixel(0,0), qRgba(0, 0, 0, 255));
	QCOMPARE(compose(black_img, white_img, multiply).pixel(0,0), qRgba(0, 0, 0, 255));
	
	QCOMPARE(compose(white_img, trans_img, multiply).pixel(0,0), qRgba(255, 255, 255, 255));
	QCOMPARE(compose(trans_img, white_img, multiply).pixel(0,0), qRgba(255, 255, 255, 255));
	
	QCOMPARE(compose(black_img, trans_img, multiply).pixel(0,0), qRgba(0, 0, 0, 255));
	QCOMPARE(compose(trans_img, black_img, multiply).pixel(0,0), qRgba(0, 0, 0, 255));
	
	QCOMPARE(compose(white_img, white_img, multiply).pixel(0,0), qRgba(255, 255, 255, 255));
	QCOMPARE(compose(black_img, black_img, multiply).pixel(0,0), qRgba(0, 0, 0, 255));
	
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 9) || (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && QT_VERSION < QT_VERSION_CHECK(6, 2, 4))
	QEXPECT_FAIL("", "CompositionMode_Multiply incorrectly composes full transparency.", Continue);
#endif
	QCOMPARE(compose(trans_img, trans_img, multiply).pixel(0,0), qRgba(0, 0, 0, 0));
	
	// ImageTransparencyFixup fixes the particular issue.
	QImage result = compose(trans_img, trans_img, multiply);
	ImageTransparencyFixup fixup(&result);
	fixup();
	QCOMPARE(result.pixel(0,0), qRgba(0, 0, 0, 0)); // Now correct!
}

void QPainterTest::darkenComposition()
{
	const QPainter::CompositionMode darken = QPainter::CompositionMode_Darken;
	
	QCOMPARE(compose(white_img, black_img, darken).pixel(0,0), qRgba(0, 0, 0, 255));
	QCOMPARE(compose(black_img, white_img, darken).pixel(0,0), qRgba(0, 0, 0, 255));
	
	QCOMPARE(compose(white_img, trans_img, darken).pixel(0,0), qRgba(255, 255, 255, 255));
	QCOMPARE(compose(trans_img, white_img, darken).pixel(0,0), qRgba(255, 255, 255, 255));
	
	QCOMPARE(compose(black_img, trans_img, darken).pixel(0,0), qRgba(0, 0, 0, 255));
	QCOMPARE(compose(trans_img, black_img, darken).pixel(0,0), qRgba(0, 0, 0, 255));
	
	QCOMPARE(compose(white_img, white_img, darken).pixel(0,0), qRgba(255, 255, 255, 255));
	QCOMPARE(compose(black_img, black_img, darken).pixel(0,0), qRgba(0, 0, 0, 255));
	
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 9) || (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && QT_VERSION < QT_VERSION_CHECK(6, 2, 4))
	QEXPECT_FAIL("", "CompositionMode_Darken incorrectly composes full transparency.", Continue);
#endif
	QCOMPARE(compose(trans_img, trans_img, darken).pixel(0,0), qRgba(0, 0, 0, 0));
	
	// ImageTransparencyFixup fixes the particular issue.
	QImage result = compose(trans_img, trans_img, darken);
	ImageTransparencyFixup fixup(&result);
	fixup();
	QCOMPARE(result.pixel(0,0), qRgba(0, 0, 0, 0)); // Now correct!
}

template <typename ColorT>
QImage QPainterTest::makeImage(ColorT color) const
{
	QImage image(1, 1, QImage::Format_ARGB32_Premultiplied);
	image.fill(color);
	return image;
}

QImage QPainterTest::compose(const QImage& source, const QImage& dest, QPainter::CompositionMode mode)
{
	QImage result = dest;
	QPainter painter(&result);
	painter.setCompositionMode(mode);
	painter.drawImage(0, 0, source);
	painter.end();
	return result;
}


QTEST_GUILESS_MAIN(QPainterTest)
