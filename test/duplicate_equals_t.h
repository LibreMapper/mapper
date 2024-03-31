/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_DUPLICATE_EQUALS_T_H
#define LIBREMAPPER_DUPLICATE_EQUALS_T_H

#include <QObject>
// IWYU pragma: no_include <QString>


/**
 * @test Test that duplicates of symbols and objects are equal to their originals.
 */
class DuplicateEqualsTest : public QObject
{
Q_OBJECT
private slots:
	void initTestCase();
	
	void symbols();
	void symbols_data();
	
	void objects();
	void objects_data();
};

#endif
