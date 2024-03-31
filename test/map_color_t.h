/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_MAP_COLOR_T_H
#define LIBREMAPPER_MAP_COLOR_T_H

#include <QObject>


/**
 * @test Tests MapColor properties.
 */
class MapColorTest : public QObject
{
Q_OBJECT
public:
	/** Constructor */
	explicit MapColorTest(QObject* parent = nullptr);
	
private slots:
	/** Tests the constructors. */
	void constructorTest();
	
	/** Tests the equals method for some simple changes. */
	void equalsTest();
	
	/** Tests spot colors */
	void spotColorTest();
	
	/** Tests other feature */
	void miscTest();
	
};

#endif
