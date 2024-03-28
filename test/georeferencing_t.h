/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_GEOREFERENCING_TEST_H
#define OPENORIENTEERING_GEOREFERENCING_TEST_H

#include <QObject>
#include <QString>


/**
 * @test Tests the Georeferencing class.
 */
class GeoreferencingTest: public QObject
{
Q_OBJECT
	
private slots:
	/**
	 * Initializes shared data.
	 */
	void initTestCase();
	
	/**
	 * Tests the behaviour for unset projected CRS.
	 */
	void testEmptyProjectedCRS();
	
	/**
	 * Tests supported state changes.
	 */
	void testStateChanges();
	
	/**
	 * Tests the effect of a grid scale factor.
	 */
	void testGridScaleFactor();
	
	void testGridScaleFactor_data();
	
	/**
	 * Tests whether Georeferencing supports particular projected CRS.
	 */
	void testCRS();
	
	void testCRS_data();
	
	/**
	 * Tests CRS templates.
	 */
	void testCRSTemplates();
	
	/**
	 * Tests whether Georeferencing can convert geographic coordinates to
	 * projected coordinates and vice versa for a collection of known points.
	 */
	void testProjection();
	
	void testProjection_data();
	
#ifndef ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
	/**
	 * Tests whether the `proj_context_set_file_finder()` function is working.
	 * 
	 * Mapper relies on this function to extract data files on Android.
	 * This test covers this function in native builds.
	 */
	void testProjContextSetFileFinder();
#endif
	
};

#endif
