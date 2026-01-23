# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2018,2025 Libor Pecháček
#
# This file is part of LibreMapper.

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

add_definitions(
  -DQT_NO_CAST_FROM_ASCII
  -DQT_NO_CAST_TO_ASCII
  -DQT_USE_QSTRINGBUILDER
)

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -fsanitize=fuzzer,address,undefined")

add_compile_options(-g -O1 -fsanitize=fuzzer,address,undefined)

# This function defines a test from a list of source file names
# (without ending). For each filename, a .cpp file and .h must exist.
# The .h file is processed with Qt's moc.
# The first filename is also used as the name of the executable file and
# as the name of the test.
#
# The test executable is linked only to external libraries by default.
# It only needs to be rebuild and run when one of its components was modified.
# Additional link libraries may be added to the executable target as usual.
#
# The autorun parameter controls whether the test will be run automatically
# when executing Mapper-with-test.
#
function(add_test_helper testname autorun)
	unset(TEST_${testname}_SRCS)
	set(manual_test 0)
	foreach(arg ${ARGN})
		if("${arg}" STREQUAL "MANUAL")
			set(manual_test 1)
		else()
			list(APPEND TEST_${testname}_SRCS ${arg}.cpp)
		endif()
	endforeach()
	add_executable(${testname} ${testname}.cpp ${TEST_${testname}_SRCS})
	target_link_libraries(${testname}
	  PROJ4::proj
	  Qt::Widgets
	  Clipper2::Clipper2
	)
endfunction()


# This function defines a unit test from a list of source file names
# (without ending). For each filename, a .cpp file and .h must exist.
# The .h file is processed with Qt's moc.
# The first filename is also used as the name of the executable file and
# as the name of the test.
#
# A unit test executable is linked only to external libraries by default.
# It only needs to be rebuild and run when one of its components was modified.
# Additional link libraries may be added to the executable target as usual.
#
function(add_unit_test testname)
	add_test_helper("${testname}" 1 ${ARGN})
endfunction()


# This function defines a system test from a list of source file names
# (without ending). For each filename, a .cpp file and .h must exist.
# The .h file is processed with Qt's moc.
# The first filename is also used as the name of the executable file and
# as the name of the test.
#
# A system test executable is linked to the full Mapper runtime.
# That is why it will be rebuild and run very often.
#
function(add_system_test testname)
	add_test_helper("${testname}" ${Mapper_AUTORUN_SYSTEM_TESTS} ${ARGN})
	target_link_libraries(${testname}
	  Mapper_Common
	)
endfunction()


# Include generated files (moc ouput, build configuration)
include_directories("${PROJECT_BINARY_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")


# Unit tests
add_system_test(fuzz-fileformat)
