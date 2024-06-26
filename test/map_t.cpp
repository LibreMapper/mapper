/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "map_t.h"

#include <QtTest>
#include <QBuffer>
#include <QMessageBox>
#include <QTextStream>

#include "test_config.h"

#include "global.h"
#include "core/map.h"
#include "core/map_color.h"
#include "core/map_printer.h" // IWYU pragma: keep
#include "core/map_view.h"
#include "core/objects/object.h"
#include "core/objects/symbol_rule_set.h"
#include "core/symbols/symbol.h"
#include "core/symbols/point_symbol.h"

using namespace LibreMapper;


namespace
{
	QDir examples_dir;    // clazy:exclude=non-pod-global-static
	QDir symbol_set_dir;  // clazy:exclude=non-pod-global-static
}


void MapTest::initTestCase()
{
	Q_INIT_RESOURCE(resources);
	
	doStaticInitializations();
	
	examples_dir.cd(QDir(QString::fromUtf8(MAPPER_TEST_SOURCE_DIR)).absoluteFilePath(QStringLiteral("../examples")));
	QVERIFY(examples_dir.exists());
	
	symbol_set_dir.cd(QDir(QString::fromUtf8(MAPPER_TEST_SOURCE_DIR)).absoluteFilePath(QStringLiteral("../symbol sets")));
	QVERIFY(symbol_set_dir.exists());
	
	// Static map initializations
	Map map;
}


void MapTest::iconTest()
{
	Map map;
	// Newly constructed map
	QVERIFY(!qIsNull(map.symbolIconZoom()));
	
	// Explicit update on newly constructed map
	map.updateSymbolIconZoom();
	const auto default_zoom = map.symbolIconZoom();
	QVERIFY(default_zoom > 0);
	
	// Single symbol, 1 mm
	auto symbol = duplicate(*map.getUndefinedPoint()).release();
	symbol->setInnerRadius(500);
	QCOMPARE(symbol->dimensionForIcon(), qreal(1));
	map.addSymbol(symbol, 0);
	map.updateSymbolIconZoom();
	// Helper symbols do not affect the symbol icon zoom.
	symbol->setIsHelperSymbol(true);
	QCOMPARE(map.symbolIconZoom(), default_zoom);
	
	// The first regular symbol affects the symbol icon zoom.
	symbol->setIsHelperSymbol(false);
	map.updateSymbolIconZoom();
	QVERIFY(!qFuzzyCompare(map.symbolIconZoom(), default_zoom));
	QCOMPARE(map.symbolIconZoom() * symbol->dimensionForIcon(), qreal(0.9)); // 90%
	
	// Change symbol size to 4 mm
	symbol->setInnerRadius(2000);
	QCOMPARE(symbol->dimensionForIcon(), qreal(4));
	map.updateSymbolIconZoom();
	QCOMPARE(map.symbolIconZoom() * symbol->dimensionForIcon(), qreal(1));
	
	map.addSymbol(duplicate(*symbol).release(), 1);
	map.updateSymbolIconZoom();
	QCOMPARE(map.symbolIconZoom() * symbol->dimensionForIcon(), qreal(1));
	
	map.addSymbol(duplicate(*symbol).release(), 2);
	map.updateSymbolIconZoom();
	QCOMPARE(map.symbolIconZoom() * symbol->dimensionForIcon(), qreal(1));
	
	QCOMPARE(map.getNumSymbols(), 3);
	symbol->setInnerRadius(100);
	QCOMPARE(symbol->dimensionForIcon(), qreal(0.2));
	map.updateSymbolIconZoom();
	// Symbol dimensions, ordered:  0.2  4.0  4.0
	// The small symbol has 5% of the size of the large symbols.
	// Zoom is expected to be adjusted order to enlarge the smallest symbol.
	// Thus the other symbol will result in more than 90% size...
	QVERIFY(map.symbolIconZoom() * 4.0 > 0.9);
	// ... while the smallest symbol will still be at most 10% size.
	QVERIFY(map.symbolIconZoom() * 0.2 <= 0.1);
}


void MapTest::printerConfigTest()
{
	Map map;
	QVERIFY(!map.hasPrinterConfig());
	map.setPrinterConfig({ map });
	QVERIFY(map.hasPrinterConfig());
	map.resetPrinterConfig();
	QVERIFY(!map.hasPrinterConfig());
}

void MapTest::specialColorsTest()
{
	QVERIFY(Map::getCoveringRed() != nullptr);
	QCOMPARE(Map::getCoveringWhite()->getPriority(), static_cast<int>(MapColor::CoveringWhite));
	
	QVERIFY(Map::getCoveringWhite() != nullptr);
	QCOMPARE(Map::getCoveringRed()->getPriority(), static_cast<int>(MapColor::CoveringRed));
	
	QVERIFY(Map::getUndefinedColor() != nullptr);
	QCOMPARE(Map::getUndefinedColor()->getPriority(), static_cast<int>(MapColor::Undefined));
	
	QVERIFY(Map::getRegistrationColor() != nullptr);
	QCOMPARE(Map::getRegistrationColor()->getPriority(), static_cast<int>(MapColor::Registration));
	
	Map map;               // non-const
	const Map& cmap(map);  // const
	QCOMPARE(map.getMapColorByPrio(MapColor::CoveringWhite), static_cast<MapColor*>(nullptr));
	QCOMPARE(cmap.getMapColorByPrio(MapColor::CoveringWhite), static_cast<MapColor*>(nullptr));
	QCOMPARE(map.getColorByPrio(MapColor::CoveringWhite), Map::getCoveringWhite());
	QCOMPARE(cmap.getColorByPrio(MapColor::CoveringWhite), Map::getCoveringWhite());
	QCOMPARE(map.getMapColorByPrio(MapColor::CoveringRed), static_cast<MapColor*>(nullptr));
	QCOMPARE(cmap.getMapColorByPrio(MapColor::CoveringRed), static_cast<MapColor*>(nullptr));
	QCOMPARE(map.getColorByPrio(MapColor::CoveringRed), Map::getCoveringRed());
	QCOMPARE(cmap.getColorByPrio(MapColor::CoveringRed), Map::getCoveringRed());
	QCOMPARE(map.getMapColorByPrio(MapColor::Undefined), static_cast<MapColor*>(nullptr));
	QCOMPARE(cmap.getMapColorByPrio(MapColor::Undefined), static_cast<MapColor*>(nullptr));
	QCOMPARE(map.getColorByPrio(MapColor::Undefined), Map::getUndefinedColor());
	QCOMPARE(cmap.getColorByPrio(MapColor::Undefined), Map::getUndefinedColor());
	QCOMPARE(map.getMapColorByPrio(MapColor::Registration), static_cast<MapColor*>(nullptr));
	QCOMPARE(cmap.getMapColorByPrio(MapColor::Registration), static_cast<MapColor*>(nullptr));
	QCOMPARE(map.getColorByPrio(MapColor::Registration), Map::getRegistrationColor());
	QCOMPARE(cmap.getColorByPrio(MapColor::Registration), Map::getRegistrationColor());
	
	QCOMPARE(map.getMapColorByPrio(MapColor::Reserved), static_cast<MapColor*>(nullptr));
	QCOMPARE(cmap.getMapColorByPrio(MapColor::Reserved), static_cast<MapColor*>(nullptr));
	QCOMPARE(map.getColorByPrio(MapColor::Reserved), static_cast<MapColor*>(nullptr));
	QCOMPARE(cmap.getColorByPrio(MapColor::Reserved), static_cast<MapColor*>(nullptr));
	
	QCOMPARE(map.getMapColorByPrio(map.getNumColorPrios()), static_cast<MapColor*>(nullptr));
	QCOMPARE(cmap.getMapColorByPrio(cmap.getNumColorPrios()), static_cast<MapColor*>(nullptr));
	QCOMPARE(map.getColorByPrio(map.getNumColorPrios()), static_cast<MapColor*>(nullptr));
	QCOMPARE(cmap.getColorByPrio(cmap.getNumColorPrios()), static_cast<MapColor*>(nullptr));
}

void MapTest::importTest_data()
{
	QTest::addColumn<QString>("first_file");
	QTest::addColumn<QString>("imported_file");

	QTest::newRow("complete map, overprinting")   << "complete map.omap" << "overprinting.omap";
	QTest::newRow("overprinting, forest sample")  << "overprinting.omap" << "forest sample.omap";
	QTest::newRow("forest sample, complete map")   << "forest sample.omap" << "complete map.omap";
}

void MapTest::importTest()
{
	QFETCH(QString, first_file);
	QFETCH(QString, imported_file);
	
	QString first_path = examples_dir.absoluteFilePath(first_file);
	Map map;
	MapView view{ &map };
	QVERIFY(map.loadFrom(first_path, &view));
	QVERIFY(map.getNumSymbols() > 0);
	
	auto original_size = map.getNumObjects();
	auto original_num_colors = map.getNumColorPrios();
	Map empty_map;
	map.importMap(empty_map, Map::CompleteImport);
	QCOMPARE(map.getNumObjects(), original_size);
	QCOMPARE(map.getNumColorPrios(), original_num_colors);
	
	map.importMap(map, Map::ColorImport);
	QCOMPARE(map.getNumObjects(), original_size);
	QCOMPARE(map.getNumColorPrios(), original_num_colors);
	
	map.importMap(map, Map::CompleteImport);
	QCOMPARE(map.getNumObjects(), 2*original_size);
	QCOMPARE(map.getNumColorPrios(), original_num_colors);
	
	QString imported_path = examples_dir.absoluteFilePath(imported_file);
	Map imported_map;
	QVERIFY(imported_map.loadFrom(imported_path));
	QVERIFY(imported_map.getNumSymbols() > 0);
	
	original_size = map.getNumObjects();
	auto symbol_map = map.importMap(imported_map, Map::CompleteImport, nullptr, -1, false);
	QCOMPARE(map.getNumObjects(), original_size + imported_map.getNumObjects());
	QCOMPARE(symbol_map.size(), imported_map.getNumSymbols());
}



void MapTest::hasAlpha()
{
	Map map;
	MapView view{ &map };
	QVERIFY(map.loadFrom(examples_dir.absoluteFilePath(QStringLiteral("complete map.omap")), &view));
	QVERIFY(!map.hasAlpha());
	
	QVERIFY(map.getNumParts() > 0);
	QVERIFY(map.getPart(0)->getNumObjects() > 0);
	
	auto* symbol = [&map]() -> Symbol* {
	               auto* symbol = map.getPart(0)->getObject(0)->getSymbol();
	               return symbol ? map.getSymbol(map.findSymbolIndex(symbol)) : nullptr;
	}();
	QVERIFY(symbol);
	
	auto* color = [&map, symbol]() -> MapColor* {
	              auto* color = symbol->guessDominantColor();
	              return color ? map.getMapColorByPrio(map.findColorPrio(color)) : nullptr;
	}();
	QVERIFY(color);
	
	color->setOpacity(0.5);
	QVERIFY(map.hasAlpha());
	
	color->setOpacity(0);
	QVERIFY(!map.hasAlpha());
	
	color->setOpacity(0.5);
	for (int i = 0; i < map.getNumSymbols(); ++i)
	{
		if (map.getSymbol(i)->containsColor(color))
			map.getSymbol(i)->setHidden(true);
	}
	QVERIFY(!map.hasAlpha());
	
	view.setAllTemplatesHidden(false);
	view.setGridVisible(false);
	QVERIFY(!view.hasAlpha());
	
	view.setMapVisibility({1, true});
	QVERIFY(!view.hasAlpha());
	
	view.setMapVisibility({1, false});
	QVERIFY(!view.hasAlpha());
	
	view.setMapVisibility({0.5, false});
	QVERIFY(!view.hasAlpha());
	
	view.setMapVisibility({0.5, true});
	QVERIFY(view.hasAlpha());
	
	view.setAllTemplatesHidden(true);
	QVERIFY(!view.hasAlpha());
	
	view.setGridVisible(true);
	MapGrid grid;
	
	grid.setColor(qRgb(0, 0, 0));
	map.setGrid(grid);
	QVERIFY(!grid.hasAlpha());
	QVERIFY(!view.hasAlpha());
	
	grid.setColor(qRgba(0, 0, 0, 128));
	map.setGrid(grid);
	QVERIFY(grid.hasAlpha());
	QVERIFY(view.hasAlpha());
}



void MapTest::crtFileTest()
{
	auto original =  symbol_set_dir.absoluteFilePath(QString::fromLatin1("src/ISOM2000_15000.xmap"));
	Map original_map;
	QVERIFY(original_map.loadFrom(original));
	QVERIFY(original_map.getNumSymbols() > 100);
	
	auto crt_data = QByteArray{
	                "101     104\n"
	                "102     105_text \n"
	                "123456  106 \n"
	};
	QBuffer buffer{&crt_data};
	buffer.open(QIODevice::ReadOnly);
	QTextStream stream{&buffer};
	auto r = SymbolRuleSet::loadCrt(stream, original_map);
	QCOMPARE(stream.status(), QTextStream::Ok);
	QCOMPARE(r.size(), std::size_t(3));
	
	QCOMPARE(r[0].type, SymbolRule::DefinedAssignment);
	QCOMPARE(r[0].symbol->getNumberAsString(), QString::fromLatin1("101"));
	QCOMPARE(r[0].query.getOperator(), ObjectQuery::OperatorSearch);
	QCOMPARE(r[0].query.tagOperands()->value, QString::fromLatin1("104"));
	QVERIFY(r[0].query.tagOperands()->key.isEmpty());
	
	QCOMPARE(r[1].type, SymbolRule::DefinedAssignment);
	QCOMPARE(r[1].symbol->getNumberAsString(), QString::fromLatin1("102"));
	QCOMPARE(r[1].query.getOperator(), ObjectQuery::OperatorSearch);
	QCOMPARE(r[1].query.tagOperands()->value, QString::fromLatin1("105_text"));
	QVERIFY(r[1].query.tagOperands()->key.isEmpty());
	
	QCOMPARE(r[2].type, SymbolRule::NoAssignment); // no 123456 in ISOM2000
	QCOMPARE(r[2].query.getOperator(), ObjectQuery::OperatorSearch);
	QCOMPARE(r[2].query.tagOperands()->value, QString::fromLatin1("106"));
	QVERIFY(r[2].query.tagOperands()->key.isEmpty());
	
	QCOMPARE(r.squeezed().size(), std::size_t(2));
	
	QBuffer out_buffer;
	out_buffer.open(QIODevice::WriteOnly);
	QTextStream out_stream{&out_buffer};
	r.writeCrt(out_stream);
	out_stream.flush();
	out_buffer.close();
	const auto result = QString::fromLatin1(out_buffer.buffer());
	QVERIFY(result.contains(QRegularExpression(QLatin1String("^101 *104$", QRegularExpression::MultilineOption))));
	QVERIFY(result.contains(QRegularExpression(QLatin1String("102 *105_text$", QRegularExpression::MultilineOption))));
	QVERIFY(!result.contains(QLatin1String("123456")));
	QVERIFY(!result.contains(QLatin1String("106")));
}



void MapTest::matchQuerySymbolNumberTest_data()
{
	QTest::addColumn<QString>("original");
	QTest::addColumn<QString>("replacement");
	QTest::addColumn<int>("matching");

	QTest::newRow("ISOM>ISMTBOM")
	        << symbol_set_dir.absoluteFilePath(QString::fromLatin1("src/ISOM2000_15000.xmap"))
	        << symbol_set_dir.absoluteFilePath(QString::fromLatin1("15000/ISMTBOM_15000.omap"))
	        << 157; // Our ISMTBOM set has a (maybe hidden) match for every ISOM symbol.
	
	QTest::newRow("ISOM>ISSOM")
	        << symbol_set_dir.absoluteFilePath(QString::fromLatin1("src/ISOM2000_15000.xmap"))
	        << symbol_set_dir.absoluteFilePath(QString::fromLatin1("src/ISSOM_5000.xmap"))
	        << 104; // Many ISOM symbol do not have a same-number counterpart in ISSOM.
	
}


void MapTest::matchQuerySymbolNumberTest()
{
	QFETCH(QString, original);
	QFETCH(QString, replacement);
	QFETCH(int, matching);
	
	Map original_map;
	QVERIFY(original_map.loadFrom(original));
	QVERIFY(original_map.getNumSymbols() > 100);
	
	auto r = SymbolRuleSet::forAllSymbols(original_map);
	QCOMPARE(r.size(), std::size_t(original_map.getNumSymbols()));
	QCOMPARE(r.squeezed().size(), std::size_t(0));
	
	r.matchQuerySymbolNumber(original_map);
	QCOMPARE(r.size(), std::size_t(original_map.getNumSymbols()));
	QCOMPARE(r.squeezed().size(), std::size_t(original_map.getNumSymbols()));
	
	Map replacement_map;
	QVERIFY(replacement_map.loadFrom(replacement));
	QVERIFY(replacement_map.getNumSymbols() > 100);
	
	r = SymbolRuleSet::forAllSymbols(original_map);
	r.matchQuerySymbolNumber(replacement_map);
	QCOMPARE(r.squeezed().size(), std::size_t(matching));
	
	std::vector<bool> symbols_in_use;
	original_map.determineSymbolsInUse(symbols_in_use);
	auto count = std::size_t(std::count(begin(symbols_in_use), end(symbols_in_use), true));
	QVERIFY(count > 10u);
	
	r = SymbolRuleSet::forUsedSymbols(original_map);
	QCOMPARE(r.size(), count);
	r.matchQuerySymbolNumber(replacement_map);
	QVERIFY(r.squeezed().size() > 10u);
}



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


QTEST_MAIN(MapTest)
