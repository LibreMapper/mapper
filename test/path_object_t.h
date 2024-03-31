/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2015, 2016, 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_PATH_OBJECT_T_H
#define LIBREMAPPER_PATH_OBJECT_T_H

#include <QObject>


/**
 * @test Tests PathObject, MapCoord(F) and VirtualPath.
 *
 * @todo Extent this test.
 */
class PathObjectTest : public QObject
{
Q_OBJECT
public:
	/** Constructor */
	explicit PathObjectTest(QObject* parent = nullptr);
	
private slots:
	void initTestCase();
	
	/** Tests MapCoordF. */
	void mapCoordTest();
	
	/** Tests VirtualPath. */
	void virtualPathTest();
	
	/** Tests PathObject constructors. */
	void constructorTest();
	
	/** Tests PathObject::copyFrom(). */
	void copyFromTest();
	void copyFromTest_data();
	
	/** Basic test for PathObject::changePathBounds(), focus on flags. */
	void changePathBoundsBasicTest();
	void changePathBoundsBasicTest_data();
	
	/** Tests PathObject::changePathBounds(). */
	void changePathBoundsTest();
	void changePathBoundsTest_data();
	
	/** Tests properties of lines after splitting. */
	void splitLineTest();
	void splitLineTest_data();
	
	/** Tests PathObject::removeFromLine(). */
	void removeFromLineTest();
	void removeFromLineTest_data();
	
	/** Tests finding intersections with calcAllIntersectionsWith(). */
	void calcIntersectionsTest();
	
	/** Tests PathCoord and SplitPathCoord for a non-trivial zero-length path. */
	void atypicalPathTest();
	
};

#endif
