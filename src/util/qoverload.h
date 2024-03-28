/* SPDX-License-Identifier: GPL-3.0-only
 *
 * This file is part of LibreMapper.
 *
 * This is a modified version of qglobal.h from the Qt Toolkit 5.7.
 *
 * Changes:
 * 2016-03-25 Kai Pastor <dg0yt@darc.de>
 * - Adjustment of legal information
 * - Reduction to qOverload related features
 */
/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2016 Intel Corporation.
** Contact: https://www.qt.io/licensing/
**
****************************************************************************/

#ifndef OPENORIENTEERING_UTIL_QOVERLOAD_H
#define OPENORIENTEERING_UTIL_QOVERLOAD_H

#include <qglobal.h>

QT_BEGIN_NAMESPACE

template <typename... Args>
struct QNonConstOverload
{
    template <typename R, typename T>
    Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...)) const Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }

    template <typename R, typename T>
    static Q_DECL_CONSTEXPR auto of(R (T::*ptr)(Args...)) Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }
};

template <typename... Args>
struct QConstOverload
{
    template <typename R, typename T>
    Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...) const) const Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }

    template <typename R, typename T>
    static Q_DECL_CONSTEXPR auto of(R (T::*ptr)(Args...) const) Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }
};

template <typename... Args>
struct QOverload : QConstOverload<Args...>, QNonConstOverload<Args...>
{
    using QConstOverload<Args...>::of;
    using QConstOverload<Args...>::operator();
    using QNonConstOverload<Args...>::of;
    using QNonConstOverload<Args...>::operator();

    template <typename R>
    Q_DECL_CONSTEXPR auto operator()(R (*ptr)(Args...)) const Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }

    template <typename R>
    static Q_DECL_CONSTEXPR auto of(R (*ptr)(Args...)) Q_DECL_NOTHROW -> decltype(ptr)
    { return ptr; }
};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304 // C++14
template <typename... Args> Q_DECL_CONSTEXPR QOverload<Args...> qOverload Q_DECL_UNUSED = {};
template <typename... Args> Q_DECL_CONSTEXPR QConstOverload<Args...> qConstOverload Q_DECL_UNUSED = {};
template <typename... Args> Q_DECL_CONSTEXPR QNonConstOverload<Args...> qNonConstOverload Q_DECL_UNUSED = {};
#endif

QT_END_NAMESPACE

#endif /* OPENORIENTEERING_UTIL_QOVERLOAD_H */
