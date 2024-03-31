/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include <QtGlobal>
#include <QtTest>
#include <QObject>
#include <QPrinterInfo>
#include <QString>

#include "core/map_printer.h"

using namespace LibreMapper;


/**
 * @test Tests printing and export features
 */
class MapPrinterTest : public QObject
{
Q_OBJECT
private slots:
	void isPrinterTest()
	{
		QPrinterInfo printer_info;
		QCOMPARE(MapPrinter::isPrinter(&printer_info), true);
		QCOMPARE(MapPrinter::isPrinter(MapPrinter::imageTarget()), false);
		QCOMPARE(MapPrinter::isPrinter(MapPrinter::kmzTarget()), false);
		QCOMPARE(MapPrinter::isPrinter(MapPrinter::pdfTarget()), false);
	}
	
};



/*
 * We don't need a real GUI window.
 * 
 * But we discovered QTBUG-58768 macOS: Crash when using QPrinter
 * while running with "minimal" platform plugin.
 */
#ifndef Q_OS_MACOS
namespace  {
	auto Q_DECL_UNUSED qpa_selected = qputenv("QT_QPA_PLATFORM", "minimal");  // clazy:exclude=non-pod-global-static
}
#endif


QTEST_MAIN(MapPrinterTest)
#include "map_printer_t.moc"  // IWYU pragma: keep
