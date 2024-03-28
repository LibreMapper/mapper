# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2017-2020 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


find_file(LICENSING_COPYRIGHT_DIR
  NAMES copyright
  PATH_SUFFIXES share/doc
  DOC "Path to directory with 3rd-party component copyright files."
)

find_file(LICENSING_COMMON_DIR
  NAMES common-licenses
  PATHS "${LICENSING_COPYRIGHT_DIR}"
  NO_DEFAULT_PATH
  DOC "Path to directory with common license files."
)

if(NOT LICENSING_COPYRIGHT_DIR OR NOT LICENSING_COMMON_DIR)
	message(STATUS "LICENSING_COPYRIGHT_DIR: ${LICENSING_COPYRIGHT_DIR}")
	message(STATUS "LICENSING_COMMON_DIR:    ${LICENSING_COMMON_DIR}")
	message(FATAL_ERROR "Both LICENSING_COPYRIGHT_DIR and LICENSING_COMMON_DIR must be set")
endif()	


# Based on OpenOrienteering superbuild as of 2020-06-21, plus libkml 2020-12-15
list(APPEND third_party_components
  boost
  giflib
  libjpeg-turbo
  libkml
  liblzma
  libopenjp2
  libpcre3
  libpng
  libtiff
  libwebp
  minizip
  poppler
  uriparser
)
find_package(Qt5Core REQUIRED QUIET)
if(NOT ${Qt5Core_VERSION} VERSION_LESS 5.9)
	list(APPEND third_party_components
	  libpcre2
	)
endif()
if(NOT APPLE)
	list(APPEND third_party_components
	  libcurl
	  libexpat
	  libsqlite
	)
endif()
if(ANDROID)
	list(APPEND third_party_components
	  freetype
	  libiconv
	)
endif()
if(CMAKE_ANDROID_STL_TYPE MATCHES "gnustl")
	list(APPEND third_party_components
	  gnustl
	)
elseif(CMAKE_ANDROID_STL_TYPE MATCHES "c\\+\\+")
	list(APPEND third_party_components
	  libc++
	)
endif()
if(APPLE)
	list(APPEND third_party_components
	  freetype
	)
endif()
if(MINGW)
	list(APPEND third_party_components
	  gcc-libs
	  mingw-w64
	  winpthreads
	)
endif()
if(WIN32)
	list(APPEND third_party_components
	  freetype
	  libiconv
	  zlib
	)
endif()

list(APPEND common_license_names
  Apache-2.0
  Artistic
  BSD
  GFDL-1.3
  GPL-1
  GPL-2
  LGPL-2
  LGPL-2.1
  LGPL-3
)


# Map component names to source package names
set(package_names
  libcurl:curl
  libexpat:expat
  libopenjp2:openjpeg2
  libpcre2:pcre2
  libpcre3:pcre3
  libsqlite:sqlite3
  libtiff:tiff
)

set(copyright_pattern "${LICENSING_COPYRIGHT_DIR}/@package@*.txt")
set(common_license_pattern "${LICENSING_COMMON_DIR}/@basename@.txt")
