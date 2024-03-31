/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps, Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

/** This file is used by qmake-driven builds only. */

#ifndef LIBREMAPPER_MAPPER_CONFIG_H
#define LIBREMAPPER_MAPPER_CONFIG_H

#if !defined(APP_NAME)
#define APP_NAME qApp->translate("OpenOrienteering::Global", QT_TRANSLATE_NOOP("Global", "OpenOrienteering Mapper"))
#endif

#if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
#define CPACK_DEBIAN_PACKAGE_NAME "openorienteering-mapper"
#define MAPPER_DEBIAN_PACKAGE_NAME CPACK_DEBIAN_PACKAGE_NAME
#endif

#endif
