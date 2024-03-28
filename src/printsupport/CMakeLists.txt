# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2015, 2016 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.

find_package(Qt5Gui REQUIRED COMPONENTS Private)
find_package(Qt5PrintSupport REQUIRED COMPONENTS Private)
find_package(ZLIB REQUIRED)

set(CMAKE_CXX_CLANG_TIDY "")
set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "")

if (${Qt5PrintSupport_VERSION} VERSION_LESS 5.12.0)
	set(FORK_VERSION qt-5.5.1)
else()
	set(FORK_VERSION qt-5.12.4)
endif()

set(PRINTSUPPORT_SRCS
  advanced_pdf_printer.cpp
  printer_properties.h
  ${FORK_VERSION}/advanced_pdf.cpp
  ${FORK_VERSION}/printengine_advanced_pdf.cpp
  ${FORK_VERSION}/qfontsubset.cpp
)

if(WIN32)
	list(APPEND PRINTSUPPORT_SRCS printer_properties_win.cpp)
else()
	list(APPEND PRINTSUPPORT_SRCS printer_properties.cpp)
endif()

add_library(printsupport STATIC ${PRINTSUPPORT_SRCS})

target_compile_definitions(printsupport PRIVATE
  QT_NO_CAST_FROM_ASCII
  QT_NO_CAST_TO_ASCII
  QT_USE_QSTRINGBUILDER
)
target_include_directories(printsupport
  SYSTEM
  BEFORE
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${FORK_VERSION}"
  INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(printsupport
  Qt5::PrintSupport
  Qt5::Gui
  ZLIB::ZLIB
)


# Workaround Qt private include dir issue
# Cf. https://bugreports.qt.io/browse/QTBUG-37417

foreach(module Core Gui PrintSupport)
	set(qt_module Qt${module})
	set(qt5_module Qt5${module})
	if("${${qt5_module}_PRIVATE_INCLUDE_DIRS}" STREQUAL "")
		foreach(base_dir ${${qt5_module}_INCLUDE_DIRS})
			if("${base_dir}" MATCHES "/${qt_module}\$")
				list(APPEND ${qt5_module}_PRIVATE_INCLUDE_DIRS "${base_dir}/${${qt5_module}_VERSION}/${qt_module}")
			endif()
		endforeach()
	endif()
	target_include_directories(printsupport SYSTEM PRIVATE ${${qt5_module}_PRIVATE_INCLUDE_DIRS})
endforeach()
