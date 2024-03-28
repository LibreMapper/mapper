/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_LOCALE_T_H
#define OPENORIENTEERING_LOCALE_T_H

#include <QObject>


/**
 * @test Tests locale and translation issues.
 */
class LocaleTest : public QObject
{
Q_OBJECT
public:
	explicit LocaleTest(QObject* parent = nullptr);
	
private slots:
	/**
	 * Tests handling of Esperanto in QLocale.
	 */
	void testEsperantoQLocale();
	
	/**
	 * Tests handling of Esperanto in TranslationUtil.
	 */
	void testEsperantoTranslationUtil();
	
};

#endif
