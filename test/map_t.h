/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_MAP_T_H
#define OPENORIENTEERING_MAP_T_H

#include <QObject>


/**
 * @test Tests the Map class. Still very incomplete
 *
 * @todo Extent this test.
 */
class MapTest : public QObject
{
Q_OBJECT
private slots:
	void initTestCase();
	
	/** Test icon scaling */
	void iconTest();
	 
	/** Tests basic print configuration properties. */
	void printerConfigTest();
	
	/** Tests if special colors are correctly handled. */
	void specialColorsTest();
	
	/** Tests various modes of Map::importMap(). */
	void importTest_data();
	void importTest();
	
	/** Tests hasAlpha() functions. */
	void hasAlpha();
	
	/** Basic tests for symbol set replacements. */
	void crtFileTest();
	
	/** Tests symbol set replacements with example files. */
	void matchQuerySymbolNumberTest_data();
	void matchQuerySymbolNumberTest();
	
};

#endif
