/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2021, 2024 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_FILE_FORMAT_T_H
#define LIBREMAPPER_FILE_FORMAT_T_H

#include <QObject>
#include <QString>


/**
 * @test Tests concerning the file formats, registry, import and export.
 * 
 */
class FileFormatTest : public QObject
{
Q_OBJECT
	
private slots:
	void initTestCase();
	
	/**
	 * Tests the MapCoord::toString() implementation which is used for export.
	 */
	void mapCoordtoString();
	void mapCoordtoString_data();
	
	/**
	 * Tests the MapCoord::fromString() implementation which is used for import.
	 */
	void mapCoordFromString();
	void mapCoordFromString_data();
	
	/**
	 * Tests filename extension fixup.
	 */
	void fixupExtensionTest();
	void fixupExtensionTest_data();
	
	/**
	 * Tests FileFormat::understands() implementations.
	 */
	void understandsTest();
	void understandsTest_data();
	
	/**
	 * Tests FileFormatRegistry::formatForData().
	 */
	void formatForDataTest();
	void formatForDataTest_data();
	
	/**
	 * Tests that high coordinates are correctly moved to the central region
	 * of the map.
	 * 
	 * \see issue #513
	 */
	void issue_513_high_coordinates();
	void issue_513_high_coordinates_data();
	
	/**
	 * Tests that maps contain the same information before and after saving
     * them and loading them again.
     */
	void saveAndLoad();
	void saveAndLoad_data();
	
	/**
	 * Tests saving and loading a map which is created in memory and does not go
	 * through an implicit export-import-cycle before the test.
	 */
	void pristineMapTest();
	
	/**
	 * Tests export of geospatial vector data via OGR.
	 */
	void ogrExportTest();
	void ogrExportTest_data();
	
	/**
	 * Tests the export of KML courses.
	 */
	void kmlCourseExportTest();
	
	/**
	 * Tests the export of IOF courses.
	 */
	void iofCourseExportTest();
	
	/**
	 * Test the creation of templates.
	 */
	void importTemplateTest_data();
	void importTemplateTest();
	
	/**
	 * Ensure proper text handling during the OCD export/import routine.
	 */
	void issue_2206_32byte_text_data();
	void issue_2206_32byte_text();

	/**
	 * Test color traits persistence and the link to symbols.
	 */
	void colorTest_data();
	void colorTest();

	/**
	 * Test text export to OCD.
	 */
	void ocdTextExportTest_data();
	void ocdTextExportTest();

	/**
	 * Test text import from OCD.
	 */
	void ocdTextImportTest_data();
	void ocdTextImportTest();
	
	/**
	 * Test path import from OCD.
	 */
	void ocdPathImportTest_data();
	void ocdPathImportTest();

};

#endif // LIBREMAPPER_FILE_FORMAT_T_H
