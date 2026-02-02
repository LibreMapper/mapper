# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2013-2015 Kai Pastor (OpenOrienteering)
# Copyright 2026 Libor Pecháček
#
# This file is part of LibreMapper.

cmake_minimum_required(VERSION 3.24)

project(Clipper2)

# Configuration options

set(CLIPPER2_VERSION_DEFAULT 2.0.1)
set(CLIPPER2_VERSION
    ${CLIPPER2_VERSION_DEFAULT}
    CACHE STRING "Version number of the Clipper2 library")
mark_as_advanced(CLIPPER2_VERSION)

message(STATUS "Configuring Clipper2 library ${CLIPPER2_VERSION}")

if(NOT ${CLIPPER2_VERSION} STREQUAL ${CLIPPER2_VERSION_DEFAULT})
  message(
    WARNING
      "The Clipper2 library version is different from the current recommended version "
      "(${CLIPPER2_VERSION} vs. ${CLIPPER2_VERSION_DEFAULT}).")
endif()

# Optionally use externally provided clipper source dir (e.g. Debian packaging)
set(CLIPPER2_SOURCE_DIR_DEFAULT)
set(CLIPPER2_SOURCE_DIR
    "${CLIPPER2_SOURCE_DIR_DEFAULT}"
    CACHE
      STRING
      "The Clipper2 library source directory to be used instead of a download")
mark_as_advanced(CLIPPER2_SOURCE_DIR)

if(CLIPPER2_SOURCE_DIR)
  # Expand variables like @PROJECT_SOURCE_DIR@
  string(CONFIGURE "${CLIPPER2_SOURCE_DIR}" EXPANDED_SOURCE_DIR @ONLY)
  set(CLIPPER2_SOURCE SOURCE_DIR "${EXPANDED_SOURCE_DIR}")
else()
  set(CLIPPER2_SHA256SUMS # Schema: VERSION:SHA256
      2.0.1:2a3693aceab4aed3e39b743e038d87701acc53cf05ed7b2013aab3e0aec5287e)
  foreach(line ${CLIPPER2_SHA256SUMS})
    if(${line} MATCHES "^${CLIPPER2_VERSION}:")
      string(REPLACE "${CLIPPER2_VERSION}:" "" CLIPPER2_SHA256 ${line})
      break()
    endif()
  endforeach()
  if(NOT CLIPPER2_SHA256)
    message(
      FATAL_ERROR
        "Unknown SHA256 sum for Clipper2 library ${CLIPPER2_VERSION}. "
        "Edit ${PROJECT_SOURCE_DIR}/CMakeLists.txt, "
        "or specify CLIPPER2_SHA256 value on the command line.")
  endif()
  set(CLIPPER2_SOURCE
      DOWNLOAD_DIR
      ${PROJECT_SOURCE_DIR}/download
      URL
      "https://github.com/AngusJohnson/Clipper2/archive/refs/tags/Clipper2_${CLIPPER2_VERSION}.tar.gz"
      URL_HASH
      SHA256=${CLIPPER2_SHA256})
endif()

set(CLIPPER2_LICENSE_FILE "${PROJECT_SOURCE_DIR}/LICENSE")
if(EXISTS "${CLIPPER2_LICENSE_FILE}.${CLIPPER2_VERSION}")
  set(CLIPPER2_LICENSE_FILE "${CLIPPER2_LICENSE_FILE}.${CLIPPER2_VERSION}")
endif()
file(GLOB CLIPPER2_LICENSE_FILES LICENSE*)
add_custom_target(
  clipper-licenses
  COMMENT "This target makes Qt Creator show license files in the project tree."
  SOURCES ${CLIPPER2_LICENSE_FILES})

# External project definition
#
# We try to reuse the Clipper2 build scripts as much as possible. The
# Clipper2Build target takes care of the cmake invocation, build and 
# install.
include(ExternalProject)
ExternalProject_Add(
  Clipper2Source
  ${CLIPPER2_SOURCE}
  CMAKE_ARGS
    "-DCLIPPER2_TESTS=OFF"
    "-DCLIPPER2_EXAMPLES=OFF"
    "-DCLIPPER2_UTILS=OFF"
    "-DCLIPPER2_USINGZ=OFF"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
    "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
    "-DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}"
    "-DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}"
    "-DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
    "-DCMAKE_CXX_FLAGS_MINSIZEREL=${CMAKE_CXX_FLAGS_MINSIZEREL}"
  SOURCE_SUBDIR "CPP"
  INSTALL_COMMAND "")

ExternalProject_Get_Property(Clipper2Source SOURCE_DIR)
ExternalProject_Get_Property(Clipper2Source BINARY_DIR)

# Imported target for consumption by the parent project
add_library(Clipper2::Clipper2 STATIC IMPORTED GLOBAL)
# Creation of the include directory makes CMake accept the imported target.
set(CLIPPER2_INCLUDE "${SOURCE_DIR}/CPP/Clipper2Lib/include")
file(MAKE_DIRECTORY "${CLIPPER2_INCLUDE}")
set_target_properties(
  Clipper2::Clipper2
  PROPERTIES IMPORTED_LOCATION "${BINARY_DIR}/libClipper2.a"
             INTERFACE_INCLUDE_DIRECTORIES "${CLIPPER2_INCLUDE}"
             INTERFACE_COMPILE_DEFINITIONS CLIPPER2_ENABLED
             INTERFACE_LINK_LIBRARIES "-lm")
add_dependencies(Clipper2::Clipper2 Clipper2Source)
