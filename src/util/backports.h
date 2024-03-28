/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_UTIL_BACKPORTS_H
#define OPENORIENTEERING_UTIL_BACKPORTS_H

#include <QtGlobal>

#if QT_VERSION < 0x050700
#  include "qasconst.h"  // IWYU pragma: export
#  include "qoverload.h" // IWYU pragma: export
#endif

#if defined(Q_OS_ANDROID) && defined(_GLIBCXX_CMATH)
namespace std
{

template <class T>
T hypot(T t1, T t2)
{
	return ::hypot(t1, t2);
}

}
#endif

#endif
