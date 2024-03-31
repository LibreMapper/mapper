/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_TOOLS_T_H
#define LIBREMAPPER_TOOLS_T_H

#include <QObject>


/**
 * @test Tests the editing tools.
 */
class ToolsTest : public QObject
{
Q_OBJECT
private slots:
	void initTestCase();
	
	void editTool();
	
	void paintOnTemplateFeature();
};

#endif
