# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2018-2020 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
find_package(Qt6 COMPONENTS Sensors Positioning SerialPort GLOBAL)
if(ANDROID)
	find_package(Qt6 COMPONENTS AndroidExtras REQUIRED)
endif()

set(CMAKE_AUTOMOC ON)


# FakePositionPlugin

set(FAKE_POSITION_DEFINITIONS
  PRIVATE
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_USE_QSTRINGBUILDER
    QT_STATICPLUGIN
)
set(FAKE_POSITION_SOURCES )
set(FAKE_POSITION_LINK_LIBRARIES )
if(TARGET Qt::Positioning)
	list(APPEND FAKE_POSITION_DEFINITIONS
	  PUBLIC MAPPER_USE_FAKE_POSITION_PLUGIN
	)
	list(APPEND FAKE_POSITION_SOURCES
	  fake_position_plugin.cpp
	  fake_position_plugin.json
	  fake_position_source.cpp
	)
	list(APPEND FAKE_POSITION_LINK_LIBRARIES
	  PRIVATE Qt::Gui Qt::Positioning
	)
endif()

qt_add_library(fake_position_source STATIC  ${FAKE_POSITION_SOURCES})
target_compile_definitions(fake_position_source  ${FAKE_POSITION_DEFINITIONS})
target_include_directories(fake_position_source  PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(fake_position_source  ${FAKE_POSITION_LINK_LIBRARIES})


# PowershellPositionPlugin
#
# For testing purposes, we build this plugin on all platforms,
# but we register it in main.cpp on WIN32 only.

set(POWERSHELL_POSITION_DEFINITIONS
  PRIVATE
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_USE_QSTRINGBUILDER
    QT_STATICPLUGIN
)
set(POWERSHELL_POSITION_SOURCES )
set(POWERSHELL_POSITION_LINK_LIBRARIES )
if(TARGET Qt::Positioning)
	list(APPEND POWERSHELL_POSITION_DEFINITIONS
	  PUBLIC MAPPER_USE_POWERSHELL_POSITION_PLUGIN
	)
	list(APPEND POWERSHELL_POSITION_SOURCES
	  powershell_position_plugin.cpp
	  powershell_position_plugin.json
	  powershell_position_source.cpp
	)
	qt6_add_resources(POWERSHELL_POSITION_SOURCES
	  "${CMAKE_CURRENT_SOURCE_DIR}/powershell_position_source.qrc"
	)
	list(APPEND POWERSHELL_POSITION_LINK_LIBRARIES
	  PRIVATE Qt::Positioning
	)
endif()

qt_add_library(powershell_position_source STATIC  ${POWERSHELL_POSITION_SOURCES})
target_compile_definitions(powershell_position_source  ${POWERSHELL_POSITION_DEFINITIONS})
target_link_libraries(powershell_position_source  ${POWERSHELL_POSITION_LINK_LIBRARIES})


# Mapper sensors

set(MAPPER_SENSORS_SOURCES
  compass.cpp
  gps_display.cpp
  gps_temporary_markers.cpp
  gps_track_recorder.cpp
  sensors_settings_page.cpp
)

set(MAPPER_SENSORS_DEFINITIONS
  PRIVATE
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_USE_QSTRINGBUILDER
)

if(TARGET Qt::Positioning OR TARGET Qt::Sensors)
	list(APPEND MAPPER_SENSORS_DEFINITIONS  PUBLIC MAPPER_USE_SENSORS)
endif()

qt_add_library(mapper-sensors STATIC  ${MAPPER_SENSORS_SOURCES})
target_compile_definitions(mapper-sensors  ${MAPPER_SENSORS_DEFINITIONS})
target_include_directories(mapper-sensors  PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(mapper-sensors
  PUBLIC
    powershell_position_source
    Qt::Core
    Qt::Gui
    Qt::Widgets
)
foreach(lib Qt::Positioning Qt::Sensors Qt::SerialPort Qt::AndroidExtras)
	if(TARGET ${lib})
		target_link_libraries(mapper-sensors  PRIVATE ${lib})
	endif()
endforeach()

if(Mapper_DEVELOPMENT_BUILD)
	target_link_libraries(mapper-sensors
	  PUBLIC
		fake_position_source
	)
endif()


# Translations

mapper_translations_sources(
  ${MAPPER_SENSORS_SOURCES}
  ${POWERSHELL_POSITION_SOURCES}
)
