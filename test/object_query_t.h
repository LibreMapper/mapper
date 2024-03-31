/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Mitchell Krome (OpenOrienteering)
 * Copyright 2017-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_OBJECT_QUERY_T_H
#define LIBREMAPPER_OBJECT_QUERY_T_H

#include <QObject>

namespace OpenOrienteering { class Object; }
using OpenOrienteering::Object;


/**
 * @test Tests object query
 */
class ObjectQueryTest : public QObject
{
Q_OBJECT
public:
	explicit ObjectQueryTest(QObject* parent = nullptr);
	
private slots:
	void testIsQuery();
	void testIsNotQuery();
	void testContainsQuery();
	void testOrQuery();
	void testAndQuery();
	void testSearch();
	void testObjectText();
	void testSymbol();
	void testNegation();
	void testToString();
	void testParser();

private:
	const Object* testObject();
};

#endif
