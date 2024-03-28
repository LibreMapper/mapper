# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2016-2020 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.

find_package(GDAL REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
 
# Extra header to be shown in the IDE or to be translated
set(MAPPER_GDAL_HEADERS
  ogr_file_format_p.h
)
	
set(MAPPER_GDAL_SOURCES
  gdal_file.cpp
  gdal_image_reader.cpp
  gdal_manager.cpp
  gdal_settings_page.cpp
  gdal_template.cpp
  kmz_groundoverlay_export.cpp
  ogr_file_format.cpp
  ogr_template.cpp
  mapper-osmconf.ini
)
	
mapper_translations_sources(${MAPPER_GDAL_HEADERS} ${MAPPER_GDAL_SOURCES})

add_library(mapper-gdal STATIC ${MAPPER_GDAL_HEADERS} ${MAPPER_GDAL_SOURCES} "${PROJECT_BINARY_DIR}/gdal/mapper-osmconf.ini")

target_compile_definitions(mapper-gdal PRIVATE
  QT_NO_CAST_FROM_ASCII
  QT_NO_CAST_TO_ASCII
  QT_USE_QSTRINGBUILDER
)

target_compile_definitions(mapper-gdal INTERFACE MAPPER_USE_GDAL)

target_include_directories(mapper-gdal SYSTEM PRIVATE "${GDAL_INCLUDE_DIR}")
target_include_directories(mapper-gdal PRIVATE "${PROJECT_SOURCE_DIR}/src")

target_link_libraries(mapper-gdal "${GDAL_LIBRARY}" Qt5::Core Qt5::Gui Qt5::Widgets Mapper_Common)

set_target_properties(mapper-gdal PROPERTIES PREFIX "")

install(FILES mapper-osmconf.ini DESTINATION "${MAPPER_DATA_DESTINATION}/gdal")

# Let mapper-osmconf.ini be found via "data:/gdal" during development.
add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/gdal/mapper-osmconf.ini"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "gdal"
  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/mapper-osmconf.ini" "gdal/mapper-osmconf.ini"
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
)


# Utility to dump GDAL format information

add_executable(mapper-gdal-info mapper_gdal_info.cpp)
target_include_directories(mapper-gdal-info SYSTEM PRIVATE "${GDAL_INCLUDE_DIR}")
target_link_libraries(mapper-gdal-info "${GDAL_LIBRARY}")

