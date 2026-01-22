# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2012, 2013, 2014 Thomas Schöps (OpenOrienteering)
# Copyright 2012-2025 Kai Pastor (OpenOrienteering)
# Copyright 2026 Libor Pecháček
#
# This file is part of LibreMapper.
 
cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(POLICY CMP0115)
    cmake_policy(SET CMP0115 NEW)
endif()
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FeatureSummary)

option(CMAKE_FIND_PACKAGE_PREFER_CONFIG "Lookup package config files before using find modules" ON)

# Project declaration

project(Mapper VERSION 1.0.0 LANGUAGES CXX C)
set(Mapper_COPYRIGHT "(C) 2026 LibreMapper developers")

math(EXPR Mapper_VERSION_CODE "${Mapper_VERSION_MAJOR} * 10000 + ${Mapper_VERSION_MINOR} * 100 + ${Mapper_VERSION_PATCH} * 2 + ${CMAKE_SIZEOF_VOID_P} / 4 - 1")

if(Mapper_VERSION_DISPLAY)
	message(STATUS "Custom version display string: \"${Mapper_VERSION_DISPLAY}\"")
elseif(CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE MATCHES "Release|MinSizeRel|RelWithDebInfo")
	set(Mapper_VERSION_DISPLAY "${CMAKE_BUILD_TYPE} ${Mapper_VERSION}")
else()
	set(Mapper_VERSION_DISPLAY "${Mapper_VERSION}")
endif()

string(REGEX REPLACE "[\\/:*?| ]" "_" Mapper_VERSION_FILESYSTEM "${Mapper_VERSION_DISPLAY}")

if(${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
	message(AUTHOR_WARNING "In-source builds are discouraged for development.")
endif()

# Build configuration options

if(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
	  "Type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
	  FORCE)
endif()

option(Mapper_DEBUG_TRANSLATIONS "Debug missing translations" OFF)

# To improve developer experience, build clipper if it is not found
set(Mapper_BUILD_CLIPPER "auto" CACHE STRING
  "Build the Clipper2 package from source, alternatives: ON, OFF"
)

option(Mapper_USE_GDAL "Use the GDAL library" ON)

if(ANDROID)
	set(Mapper_WITH_COVE_DEFAULT OFF)
else()
	set(Mapper_WITH_COVE_DEFAULT ON)
endif()
option(Mapper_WITH_COVE "Build and include contour line vectorization" ${Mapper_WITH_COVE_DEFAULT})

if(CMAKE_BUILD_TYPE MATCHES Release|MinSizeRel|RelWithDebInfo)
	set(Mapper_DEVELOPMENT_BUILD_DEFAULT OFF)
else()
	set(Mapper_DEVELOPMENT_BUILD_DEFAULT ON)
endif()
option(Mapper_DEVELOPMENT_BUILD "Configure development build (loading resource from the build directory)" ${Mapper_DEVELOPMENT_BUILD_DEFAULT})
mark_as_advanced(Mapper_DEVELOPMENT_BUILD)

option(Mapper_AUTORUN_SYSTEM_TESTS "Run the system tests as part of the Mapper_Test target" ${Mapper_DEVELOPMENT_BUILD})
option(Mapper_AUTORUN_MANUAL_TESTS "Run the system tests as part of the Mapper_Test target" OFF)
mark_as_advanced(Mapper_AUTORUN_SYSTEM_TESTS Mapper_AUTORUN_MANUAL_TESTS)

if(ANDROID OR APPLE OR WIN32)
	set(mapper_package_default ON)
else()
	set(mapper_package_default OFF)
endif()
if(Mapper_USE_GDAL)
	set(mapper_package_gdal ${mapper_package_default})
else()
	set(mapper_package_gdal OFF)
endif()
if(NOT ANDROID)
	set(mapper_package_assistant ${mapper_package_default})
else()
	set(mapper_package_assistant OFF)
endif()
option(Mapper_PACKAGE_PROJ      "Include all required Proj components in the packages" ${mapper_package_default})
option(Mapper_PACKAGE_GDAL      "Include all required GDAL components in the packages" ${mapper_package_gdal})
option(Mapper_PACKAGE_QT        "Include all required Qt components in the packages"   ${mapper_package_default})
option(Mapper_PACKAGE_ASSISTANT "Include Qt Assistant in the packages"                 ${mapper_package_assistant})
mark_as_advanced(Mapper_PACKAGE_PROJ Mapper_PACKAGE_GDAL Mapper_PACKAGE_QT Mapper_PACKAGE_ASSISTANT)


# Installation configuration

set(Mapper_PACKAGE_NAME "libremapper" CACHE STRING
  "The package name"
)

# These value are used for the DESTINATION parameter of the install command
# and must not be empty.
if(WIN32)
	set(MAPPER_RUNTIME_DESTINATION .)
	set(MAPPER_LIBRARY_DESTINATION .)
	set(MAPPER_DATA_DESTINATION    .)
	set(MAPPER_ABOUT_DESTINATION   "doc")
elseif(APPLE)
	set(MAPPER_RUNTIME_DESTINATION .)
	set(MAPPER_LIBRARY_DESTINATION .)
	set(MAPPER_DATA_DESTINATION    "Mapper.app/Contents/Resources")
	set(MAPPER_ABOUT_DESTINATION   "Mapper.app/Contents/Resources/doc")
elseif(ANDROID)
	set(MAPPER_RUNTIME_DESTINATION "libs/${CMAKE_ANDROID_ARCH_ABI}")
	set(MAPPER_LIBRARY_DESTINATION "libs/${CMAKE_ANDROID_ARCH_ABI}")
	set(MAPPER_DATA_DESTINATION    "assets")
	set(MAPPER_ABOUT_DESTINATION   "assets/doc")
else() # LINUX and alike
	include(GNUInstallDirs)
	set(MAPPER_RUNTIME_DESTINATION "${CMAKE_INSTALL_BINDIR}")
	set(MAPPER_LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}/${Mapper_PACKAGE_NAME}")
	set(MAPPER_DATA_DESTINATION    "${CMAKE_INSTALL_DATADIR}/${Mapper_PACKAGE_NAME}")
	string(REPLACE "/${PROJECT_NAME}" "/${Mapper_PACKAGE_NAME}"
	    MAPPER_ABOUT_DESTINATION   "${CMAKE_INSTALL_DOCDIR}")
endif()

if(CMAKE_CROSSCOMPILING)
	message(STATUS "Crosscompiling, host: ${CMAKE_HOST_SYSTEM_NAME}, target: ${CMAKE_SYSTEM_NAME}")
endif()


# Build definitions

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wpedantic")
endif()

add_custom_target(Mapper_prerequisites
  SOURCES # Extra files to be shown in IDE
    INSTALL.md
    README.md
)
set(Mapper_prerequisites_FOUND TRUE)

if(Mapper_BUILD_CLIPPER STREQUAL "auto")
	find_package(Clipper2 QUIET)
	if(NOT Clipper2_FOUND)
		message(WARNING
		  "System Clipper2 is missing. Enabling embedded build.\n"
		  "Set Mapper_BUILD_CLIPPER=OFF to disable embedded build."
		)
		set_property(CACHE Mapper_BUILD_CLIPPER PROPERTY VALUE "ON")
	else()
		set_property(CACHE Mapper_BUILD_CLIPPER PROPERTY VALUE "OFF")
	endif()
	set_property(CACHE Mapper_BUILD_CLIPPER PROPERTY TYPE "BOOL")
endif()
if(Mapper_BUILD_CLIPPER)
	add_subdirectory(3rd-party/clipper2)
	add_feature_info(Mapper_BUILD_CLIPPER 1 "version: ${CLIPPER2_VERSION}")
else()
	find_package(Clipper2 REQUIRED)
endif()

find_package(PROJ 9.3 REQUIRED)

if(Mapper_USE_GDAL)
        find_package(GDAL 3.9 REQUIRED)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()

if(CMAKE_CXX_BYTE_ORDER STREQUAL "BIG_ENDIAN")
	add_definitions(-DMAPPER_BIG_ENDIAN)
endif()


add_definitions(-D_USE_MATH_DEFINES -DUNICODE)

if(Mapper_DEVELOPMENT_BUILD)
	add_definitions(-DMAPPER_DEVELOPMENT_BUILD)
	include(EnableSanitize)
	enable_sanitize(address undefined)
	configure_file(suppress.txt.in suppress.txt COPYONLY)
else()
	add_definitions(-DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT -DNDEBUG)
endif()


# Build-time code checks

if(NOT CMAKE_HOST_WIN32)
	find_program(SH_EXECUTABLE NAMES sh PATHS ENV PATH NO_DEFAULT_PATH)
endif()

if(NOT CMAKE_CXX_CLANG_TIDY AND Mapper_DEVELOPMENT_BUILD AND SH_EXECUTABLE)
	set(min_version 8)
        find_package(ClangTidy ${min_version} QUIET)
	if(ClangTidy_FOUND)
		set(CMAKE_CXX_CLANG_TIDY "${SH_EXECUTABLE};-e;${PROJECT_SOURCE_DIR}/code-check-wrapper.sh;${ClangTidy_EXECUTABLE}")
	else()
		message(AUTHOR_WARNING "Install clang-tidy >= ${min_version} for code checks during build")
	endif()
endif()

if(NOT CMAKE_CXX_INCLUDE_WHAT_YOU_USE AND Mapper_DEVELOPMENT_BUILD AND SH_EXECUTABLE)
	set(min_version 0.12)
        find_package(IWYU ${min_version} QUIET)
	if(IWYU_FOUND)
		set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${SH_EXECUTABLE};-e;${PROJECT_SOURCE_DIR}/code-check-wrapper.sh;${IWYU_EXECUTABLE}")
	else()
		message(AUTHOR_WARNING "Install include-what-you-use >= ${min_version} for code checks during build")
	endif()
endif()


# Subdirectories

#add_subdirectory("doc/manual")
add_subdirectory("examples")
add_subdirectory("symbol sets")
add_subdirectory("translations")
if(Mapper_WITH_COVE)
	add_feature_info(Mapper_WITH_COVE "${Mapper_WITH_COVE}" "Contour line vectorization")
	add_subdirectory("3rd-party/cove")
endif()
if(NOT ANDROID)
	add_subdirectory("3rd-party/qtsingleapplication")
endif()
if (Mapper_USE_GDAL)
	add_subdirectory("src/gdal")
endif()
if(NOT ANDROID)
	add_subdirectory("src/printsupport")
endif()
add_subdirectory("src/sensors")
add_subdirectory("src")
add_subdirectory("packaging")
add_subdirectory("doc/licensing")

if(CMAKE_CROSSCOMPILING)
	add_custom_target(TEST_WARNING ALL
	  COMMENT "Crosscompiling, skipping all tests")
	add_dependencies(TEST_WARNING Mapper)
else()
	enable_testing()
	add_subdirectory("test")
endif()

#add_subdirectory("doc/api")
add_subdirectory("packaging/src")

# Configuration report

feature_summary(INCLUDE_QUIET_PACKAGES WHAT ALL)

string(TOUPPER "${CMAKE_BUILD_TYPE}" type)
foreach(lang CXX C)
	foreach(i "" _${type})
		message(STATUS "CMAKE_${lang}_FLAGS${i}: ${CMAKE_${lang}_FLAGS${i}}")
	endforeach()
endforeach()
