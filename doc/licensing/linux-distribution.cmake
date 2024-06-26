# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2017 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


# Linux distributions provide packages of programs, shared libraries and data
# which Mapper just uses, not distributes. Most dependencies needed by Mapper
# have no attribution requirements for pure usage.
#
# Commented-out items either 
# - are components which explicitly need attribution, or
# - are transitive dependencies which were checked and are recorded here for
#   reference, but do not need to be handled explicitly unless added by some
#   provider file, or
# - were not checked yet (marked by '?').
#
# Based on CMakeLists.txt component names and
# - Debian jessie
# - Debian stretch (testing) as of 2017-04
# - openSUSE Leap 42.3
# basename[:package_name] 

list(APPEND easy_dependencies
  libpolyclipping
  proj
  qtbase
  qtimageformats
  qttranslations
  # qtandroidextras ?
  qtlocation
  qtsensors
  qtserialport
  # qtsingleapplication : Linked statically, attribution required!
  # gdal dependencies
    # libarmadillo4
    # libatomic1
    # libcurl[3]:curl
    # libdap11:libdap # not used here
    # libdapclient3:libdap # not used here
    # libdapserver7:libdap # not used here
    # libepsilon1:libepsilon
    # libexpat:expat
    # libfreexl1
    # libgeos_c:geos
    # libgeotiff:libgeotiff2
    # libgif:libgif6
    # libhdf4
    # libhdf5:libhdf5-10
    # libjasper1
    # libjpeg:libjpeg-turbo
    # libjson-c3
    # libkml0
    # libkmlbase1:libkml
    # libkmldom1:libkml
    # libkmlengine1:libkml
    # libmysqlclient
    # libnetcdf[7] # not used here
    # libodbc:unixODBC
    # libodbcinst:unixODBC
    # libogdi3.2 # not used here
    # libopenjp2
    # libpcre
    # libpng16
    # libpoppler
    # libpq
    # libproj*
    # libqhull7
    # libspatialite
    # libsqlite3
    # libsz2
    # libtiff
      # liblzma5:xz
    # liburiparser1
    # libwebp
    # libxerces-c
    # libxml2
    # libz
  zlib
  # qttools ?
)

foreach(dependency ${easy_dependencies})
	if(dependency MATCHES ":")
		string(REGEX REPLACE ".*:" "" package ${dependency})
		string(REGEX REPLACE ":.*" "" dependency ${dependency})
	endif()
	if(NOT explicit_copyright_${dependency})
		set(explicit_copyright_${dependency} "${dependency}" "-")
		if(dependency MATCHES "^qt")
			find_package(Qt5Core)
			list(APPEND explicit_copyright_${dependency} "${Qt5Core_VERSION}")
		elseif(dependency STREQUAL "libpolyclipping")
			find_package(Polyclipping)
			list(APPEND explicit_copyright_${dependency} "${POLYCLIPPING_VERSION}")
		elseif(dependency STREQUAL "proj" AND PROJ_VERSION)
			list(APPEND explicit_copyright_${dependency} "${PROJ_VERSION}")
		elseif(dependency STREQUAL "zlib")
			find_package(ZLIB)
			list(APPEND explicit_copyright_${dependency} "${ZLIB_VERSION_STRING}")
		endif()
	endif()
endforeach()
