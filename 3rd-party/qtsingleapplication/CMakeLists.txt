# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2012-2015 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.
 
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_CLANG_TIDY "")
set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "")

set(QSINGLEAPP_SRCS
  src/qtlocalpeer.cpp
  src/qtsingleapplication.cpp
  src/qtsinglecoreapplication.cpp
)

add_library(QtSingleApplication STATIC
  ${QSINGLEAPP_SRCS}
)

target_include_directories(QtSingleApplication
  SYSTEM
  INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(QtSingleApplication
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Network
)
