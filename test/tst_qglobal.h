/* SPDX-License-Identifier: GPL-3.0-only
 *
 * This file is part of LibreMapper.
 *
 * This is a modified version of tst_qglobal.cpp from the Qt Toolkit 5.7.
 *
 * Changes:
 * 2016-03-25 Kai Pastor <dg0yt@darc.de>
 * - Adjustment of legal information
 * - Reduction to qOverload related tests
 * - Adaption to our testing framework
 */
/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
****************************************************************************/

#ifndef OPENORIENTEERING_TST_QGLOBAL_H
#define OPENORIENTEERING_TST_QGLOBAL_H

#include <QObject>


class tst_QGlobal: public QObject
{
    Q_OBJECT

private slots:
    void testqOverload();
};

#endif // OPENORIENTEERING_TST_QGLOBAL_H
