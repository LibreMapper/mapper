/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2020 Kai Pastor
 *
 * This file is part of CoVe.
 */

#include <algorithm>
#include <iterator>

#include <Qt>
#include <QtTest>
#include <QColor>
#include <QImage>
#include <QObject>
#include <QRgb>
#include <QSize>
#include <QString>
#include <QVector>

#include "libvectorizer/ParallelImageProcessing.h"
// IWYU pragma: no_include "libvectorizer/ProgressObserver.h"

using namespace cove;

class ParallelImageProcessingTest : public QObject
{
	Q_OBJECT
	
private slots:
	void inplaceImageTest()
	{
		auto original = QImage(64,64, QImage::Format_RGB32);
		
		// QImage: detaching unless explicitly constructed from the bits.
		{
			original.fill(Qt::white);
			QCOMPARE(original.pixel(32,32), QColor(Qt::white).rgb());
			
			auto qimage_clone = QImage(original.bits(), original.width(), original.height(), original.bytesPerLine(), original.format());
			qimage_clone.fill(Qt::red);
			QCOMPARE(original.pixel(32,32), QColor(Qt::red).rgb());
			
			auto qimage_clone_clone = QImage(qimage_clone.bits(), qimage_clone.width(), qimage_clone.height(), qimage_clone.bytesPerLine(), qimage_clone.format());
			qimage_clone_clone.fill(Qt::black);
			QCOMPARE(original.pixel(32,32), QColor(Qt::black).rgb());
			
			// A QImage which is copy-constructed from a bits-cloned QImage detaches when modified.
			qimage_clone_clone = QImage(qimage_clone);
			QVERIFY(qimage_clone_clone.bits() != original.bits());     // already detached!
			qimage_clone_clone.fill(Qt::white);
			QVERIFY(qimage_clone_clone.bits() != original.bits());     // detached!
			QCOMPARE(original.pixel(32,32), QColor(Qt::black).rgb());  // unchanged!
		}
	}
	
	
	void makeStripeTest_data()
	{
		QTest::addColumn<QSize>("size");
		QTest::addColumn<int>("scanline");
		QTest::addColumn<int>("stripe_height");
		QTest::addColumn<int>("expected_height");
		QTest::newRow("stripe at begin") << QSize(100, 100) <<  0 << 50 << 50;
		QTest::newRow("stripe at end")   << QSize(100, 100) << 75 << 25 << 25;
		QTest::newRow("stripe over end") << QSize(100, 100) << 75 << 35 << 25;
	}
	
	void makeStripeTest()
	{
		QFETCH(QSize, size);
		QFETCH(int, scanline);
		QFETCH(int, stripe_height);
		QFETCH(int, expected_height);
		
		auto original = QImage(size, QImage::Format_Indexed8);
		original.setColorTable({ QColor(Qt::red).rgb(), QColor(Qt::black).rgb() });
		original.fill(0);
		
		auto const expected_color_table = original.colorTable();
		QCOMPARE(expected_color_table.size(), 2);
		
		using std::begin;
		using std::end;
		
		{
			auto stripe_from_const = HorizontalStripes::makeStripe(static_cast<const QImage&>(original), scanline, stripe_height);
			QCOMPARE(stripe_from_const.format(), original.format());
			QCOMPARE(stripe_from_const.constBits(), original.constScanLine(scanline));
			QCOMPARE(stripe_from_const.bytesPerLine(), original.bytesPerLine());
			QCOMPARE(stripe_from_const.height(), expected_height);
			auto const color_table = stripe_from_const.colorTable();
			QVERIFY(std::equal(begin(color_table), end(color_table), begin(expected_color_table), end(expected_color_table)));
			
			QCOMPARE(original.pixelIndex(0, scanline), 0);
			QCOMPARE(stripe_from_const.pixelIndex(0, 0), 0);
			stripe_from_const.fill(1);
			QCOMPARE(stripe_from_const.pixelIndex(0, 0), 1);     // new fill color
			QCOMPARE(original.pixelIndex(0, scanline), 0);       // unchanged
		}
		
		{
			auto stripe_from_nonconst = HorizontalStripes::makeStripe(original, scanline, stripe_height);
			QCOMPARE(stripe_from_nonconst.format(), original.format());
			QCOMPARE(stripe_from_nonconst.constBits(), original.constScanLine(scanline));
			QCOMPARE(stripe_from_nonconst.bytesPerLine(), original.bytesPerLine());
			QCOMPARE(stripe_from_nonconst.height(), expected_height);
			auto const color_table = stripe_from_nonconst.colorTable();
			QVERIFY(std::equal(begin(color_table), end(color_table), begin(expected_color_table), end(expected_color_table)));
			
			QCOMPARE(original.pixelIndex(0, scanline), 0);
			QCOMPARE(stripe_from_nonconst.pixelIndex(0, 0), 0);
			stripe_from_nonconst.fill(1);
			QCOMPARE(stripe_from_nonconst.pixelIndex(0, 0), 1);  // new fill color
			QCOMPARE(original.pixelIndex(0, scanline), 1);       // new fill color
		}
	}
	
};
	
QTEST_GUILESS_MAIN(ParallelImageProcessingTest)
#include "ParallelImageProcessingTest.moc"  // IWYU pragma: keep
