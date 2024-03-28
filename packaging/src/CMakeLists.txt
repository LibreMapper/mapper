# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2013, 2014, 2018 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.
 
message(STATUS "Configuring ${PROJECT_NAME} source packaging")

set(Mapper_Source_PREFIX
  "openorienteering-mapper-${Mapper_VERSION}")
	
set(Mapper_Source_FILE_NAME
  "openorienteering-mapper_${Mapper_VERSION}-src")
	
set(Mapper_Source_FORMAT tgz CACHE STRING
  "The archive format for source packages (see `git archive -l` for valid options)")

find_program(GIT_EXECUTABLE git)
if(GIT_EXECUTABLE)
	configure_file(Mapper_Source.cmake.in Mapper_Source.cmake @ONLY)
	add_custom_target(Mapper_Source
	  COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/Mapper_Source.cmake"
	)
endif()
