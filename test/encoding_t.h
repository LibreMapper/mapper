/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_ENCODING_T_H
#define OPENORIENTEERING_ENCODING_T_H

#include <QObject>


/**
 * @test Tests encoding issues.
 */
class EncodingTest : public QObject
{
Q_OBJECT
public:
	explicit EncodingTest(QObject* parent = nullptr);
	
private slots:
	/**
	 * Tests Util::codepageForLanguage for some languages with known encoding.
	 */
	void testCodepageForLanguage();
	void testCodepageForLanguage_data();
	
	/**
	 * Tests Util::codecForName for some codecs and for "Default".
	 */
	void testCodecForName();
	
};

#endif
