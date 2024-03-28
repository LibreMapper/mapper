/* SPDX-License-Identifier: GPL-3.0-only
 *
 * This file is part of LibreMapper.
 *
 * This is a modified version of qglobal.h from the Qt Toolkit 5.7.
 *
 * Changes:
 * 2016-03-25 Kai Pastor <dg0yt@darc.de>
 * - Adjustment of legal information
 * - Reduction to qAsConst feature
 */
/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2016 Intel Corporation.
** Contact: https://www.qt.io/licensing/
**
****************************************************************************/

#ifndef OPENORIENTEERING_UTIL_QASCONST_H
#define OPENORIENTEERING_UTIL_QASCONST_H

#include <qglobal.h>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
template <typename T> struct QAddConst { typedef const T Type; };
}

// this adds const to non-const objects (like std::as_const)
template <typename T>
Q_DECL_CONSTEXPR typename QtPrivate::QAddConst<T>::Type &qAsConst(T &t) Q_DECL_NOTHROW { return t; }
// prevent rvalue arguments:
template <typename T>
void qAsConst(const T &&) Q_DECL_EQ_DELETE;

QT_END_NAMESPACE

#endif /* OPENORIENTEERING_UTIL_QASCONST_H */
