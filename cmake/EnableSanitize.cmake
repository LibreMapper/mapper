# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2016 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


if(NOT COMMAND check_cxx_compiler_flag)
	include(CheckCXXCompilerFlag)
endif()

function(enable_sanitize)
	foreach(option ${ARGV})
		if(CMAKE_CROSSCOMPILING)
			break() # out of macro
		endif()
		if(option STREQUAL "NO_RECOVER")
			set(full_option "-fno-sanitize-recover=all -fno-omit-frame-pointer")
		else()
			set(full_option "-fsanitize=${option}")
		endif()
		if(NOT CMAKE_CXX_FLAGS MATCHES "${full_option}")
			set(CMAKE_REQUIRED_LIBRARIES "${full_option}")
			string(MAKE_C_IDENTIFIER ${option} option_id)
			check_cxx_compiler_flag("${full_option}" SANITIZE_${option_id})
			if(SANITIZE_${option_id})
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${full_option}" PARENT_SCOPE)
				set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${full_option}" PARENT_SCOPE)
			endif()
		endif()
	endforeach()
endfunction()

