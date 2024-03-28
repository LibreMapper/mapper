/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#include <QObject>
#include <QString>

#include "libvectorizer/Polygons.h"

class PolygonTest : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	
	void testJoins_data();
	void testJoins();

private:
	void saveResults(const cove::PolygonList& polys,
	                 const QString& filename) const;
	void compareResults(const cove::PolygonList& polys,
	                    const QString& filename) const;
};
