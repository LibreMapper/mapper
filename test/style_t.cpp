/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include <Qt>
#include <QtGlobal>
#include <QtPlugin>
#include <QtTest>
#include <QCommonStyle>
#include <QIcon>
#include <QObject>
#include <QPixmap>
#include <QSize>
#include <QStaticPlugin>
#include <QString>
#include <QStyle>

#include "core/map_coord.h"
#include "gui/scaling_icon_engine.h"
#include "gui/widgets/mapper_proxystyle.h"

using namespace LibreMapper;



/**
 * @test Tests style customizations.
 */
class StyleTest : public QObject
{
Q_OBJECT
private slots:
	void scalingIconEngineTest();
	void standardIconTest();
};


/**
 * Tests key behaviours of ScalingIconEngine.
 */
void StyleTest::scalingIconEngineTest()
{
	QPixmap pm(4, 4);
	pm.fill(Qt::red);
	{
		// Testing Qt's default behaviour:
		QIcon icon(pm);
		// Scaling down
		QCOMPARE(icon.actualSize(QSize(1,1)), QSize(1,1));
		QCOMPARE(icon.pixmap(QSize(1,1)).size(), QSize(1,1));
		// Not scaling up
		QCOMPARE(icon.actualSize(QSize(10,10)), QSize(4,4));
		QCOMPARE(icon.pixmap(QSize(10,10)).size(), QSize(4,4));
	}
	{
		// ScalingIconEngine:
		QIcon icon(new ScalingIconEngine(QString{}));
		icon.addPixmap(pm);
		// Scaling down
		QCOMPARE(icon.actualSize(QSize(1,1)), QSize(1,1));
		QCOMPARE(icon.pixmap(QSize(1,1)).size(), QSize(1,1));
		// Scaling up
		QCOMPARE(icon.actualSize(QSize(10,10)), QSize(10,10));
		QCOMPARE(icon.pixmap(QSize(10,10)).size(), QSize(10,10));
	}
}

/**
 * Tests standard icon behaviours of MapperProxyStyle
 */
void StyleTest::standardIconTest()
{
	// CommonStyle doesn't use PNG for QStyle::SP_TitleBarMenuButton,
	// so it is not passed through ScalingIconEngine.
	auto const standard_icon = QStyle::SP_TitleBarMenuButton;
	auto const large = QSize(1000, 1000);
	{
		// Testing Qt's default behaviour:
		auto const size = QCommonStyle().standardIcon(standard_icon).actualSize(large);
		QVERIFY(size.width() < large.width());
		QVERIFY(size.height() < large.height());
	}
	{
		// ScalingIconEngine:
		// MapperProxyStyle must ensure use of the scaling icon engine
		QCOMPARE(MapperProxyStyle().standardIcon(standard_icon, nullptr, nullptr).actualSize(large), large);
	}
}


/*
 * We select a non-standard QPA because we don't need a real GUI window.
 * 
 * Normally, the "offscreen" plugin would be the correct one.
 * However, it bails out with a QFontDatabase error (cf. QTBUG-33674)
 */
namespace  {
	auto Q_DECL_UNUSED qpa_selected = qputenv("QT_QPA_PLATFORM", "minimal");  // clazy:exclude=non-pod-global-static
}


Q_IMPORT_PLUGIN(ScalingIconEnginePlugin)

QTEST_MAIN(StyleTest)

#include "style_t.moc"  // IWYU pragma: keep
