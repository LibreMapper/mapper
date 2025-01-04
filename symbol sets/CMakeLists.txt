# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2012 Thomas Schöps (OpenOrienteering)
# Copyright 2012-2019 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.

if(ANDROID)
	# Don't install symbol sets on Android
	# \todo Add map creation to the mobile app.
	return()
endif()

set(Mapper_SYMBOL_SETS
  "10000/ISOM 2017-2_10000.omap"
  "15000/ISOM 2017-2_15000.omap"
  
  "4000/ISSprOM 2019_4000.omap"
  
  5000/ISMTBOM_5000.omap
  7500/ISMTBOM_7500.omap
  10000/ISMTBOM_10000.omap
  15000/ISMTBOM_15000.omap
  20000/ISMTBOM_20000.omap
  
  "5000/ISSkiOM 2019_5000.omap"
  "7500/ISSkiOM 2019_7500.omap"
  "10000/ISSkiOM 2019_10000.omap"
  "12500/ISSkiOM 2019_12500.omap"
  "15000/ISSkiOM 2019_15000.omap"
  
  4000/Course_Design_4000.omap
  5000/Course_Design_5000.omap
  10000/Course_Design_10000.omap
  15000/Course_Design_15000.omap
)

set(Mapper_CRT_FILES
  "./ISOM 2017-2-ISSkiOM 2019.crt"
  "./ISOM 2017-2-ISSprOM 2019.crt"
  "./ISOM2000-ISOM 2017-2.crt"
  "./ISSOM-ISSprOM 2019.crt"
  "./OSM-ISOM 2017-2.crt"
  "./OSM-ISSkiOM 2019.crt"
  "./OSM-ISSprOM 2019.crt"
)

set(symbol_sets)
foreach(_symbol_set ${Mapper_SYMBOL_SETS} ${Mapper_CRT_FILES})
	cmake_path(GET _symbol_set PARENT_PATH _dir)
	cmake_path(SET _dest_path NORMALIZE "${MAPPER_DATA_DESTINATION}/symbol sets/${_dir}")
	install(FILES ${_symbol_set} DESTINATION "${_dest_path}")

	# Files in the build tree may be overwritten or deleted.
	# Rename the symbol sets in order to give a minimal warning.
	cmake_path(GET _symbol_set FILENAME _name)
	set(_copy_name ${_dir}/COPY_OF_${_name})
	add_custom_command(OUTPUT ${_copy_name}
	  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${_symbol_set} ${CMAKE_CURRENT_BINARY_DIR}/${_copy_name}
	  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_symbol_set}
	  VERBATIM)
	list(APPEND symbol_sets "${CMAKE_CURRENT_BINARY_DIR}/${_copy_name}")
endforeach()

# Copy symbol sets to the build tree
add_custom_target(Mapper_symbol_sets
  ALL
  DEPENDS ${symbol_sets}
)

