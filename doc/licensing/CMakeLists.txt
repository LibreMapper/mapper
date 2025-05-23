# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2014-2019 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


message(STATUS "Configuring ${PROJECT_NAME} licensing documentation")


# Show all available providers in Qt Creator
set(known_providers )
file(GLOB provider_files "${CMAKE_CURRENT_LIST_DIR}/*-licensing.cmake")
foreach(file ${provider_files})
	cmake_path(GET file FILENAME name)
	string(REPLACE "-licensing.cmake" "" provider "${name}")
	list(APPEND known_providers "${provider}")
	file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.stamp"
	  INPUT "${file}"
	)
endforeach()

# Select default provider
if(ANDROID OR APPLE OR WIN32)
	set(default_provider OFF)
elseif(UNIX)
	set(default_provider NOTFOUND)
	execute_process(
	  COMMAND sh -c ". /etc/os-release; echo \"$ID $ID_LIKE\""
	  OUTPUT_VARIABLE system_ids
	  OUTPUT_STRIP_TRAILING_WHITESPACE 
	)
	string(REPLACE " " ";" system_ids "${system_ids}")
	foreach(system IN LISTS system_ids)
		if(system IN_LIST known_providers)
			set(default_provider "${system}")
			break()
		endif()
	endforeach()
endif()

set(LICENSING_PROVIDER "${default_provider}" CACHE STRING
  "Provider for 3rd-party licensing information (${known_providers};OFF)"
)


# Software and data used by Mapper, as given by configuration
set(LICENSING_ATTRIBUTION "" CACHE STRING
  "Third-party components for which attribution must be deployed"
)

# A way to remove attributions which is required by default.
set(LICENSING_NO_ATTRIBUTION "" CACHE STRING
  "Third-party components for which attribution shall not be deployed"
)


# Import the targets of all found packages into this scope.
get_cmake_property(packages PACKAGES_FOUND)
find_package(Qt6 COMPONENTS REQUIRED Network)
foreach(package ${packages})
	find_package("${package}")
endforeach()


# Software and data used directly by Mapper, or set by configuration
set(third_party_components
  ${LICENSING_ATTRIBUTION}
  libpolyclipping
  proj
  qtbase
  qtimageformats
  qttranslations
)
if(TARGET Qt6::AndroidExtras)
	list(APPEND third_party_components qtandroidextras)
endif()
if(TARGET Qt6::Positioning)
	list(APPEND third_party_components qtlocation)
endif()
if(TARGET Qt6::Sensors)
	list(APPEND third_party_components qtsensors)
endif()
if(TARGET Qt6::SerialPort)
	list(APPEND third_party_components qtserialport)
endif()
if(TARGET QtSingleApplication)
	list(APPEND third_party_components qtsingleapplication)
endif()
if(TARGET mapper-gdal)
	list(APPEND third_party_components gdal)
endif()
if(TARGET printsupport AND WIN32)
	list(APPEND third_party_components zlib)
endif()
if(Mapper_PACKAGE_ASSISTANT)
	list(APPEND third_party_components qttools)
endif()
if(TARGET cove)
	list(APPEND third_party_components potrace)
endif()

message(STATUS "Direct third-party components: ${third_party_components}")


# Licences used directly by Mapper
set(common_license_names
  GPL-3
)

# Required, because linked from About dialog
set(explicit_copyright_GPL-3
  "GPL-3.txt"
  "${PROJECT_SOURCE_DIR}/COPYING"
  "common-licenses"
)
set(explicit_copyright_qtsingleapplication
  "qtsingleapplication.txt"
  "${PROJECT_SOURCE_DIR}/3rd-party/qtsingleapplication/copyright"
  "3rd-party"
)
set(explicit_copyright_potrace
  "potrace-1.6.txt"
  "${PROJECT_SOURCE_DIR}/3rd-party/cove/potrace/copyright"
  "3rd-party"
)
if(Mapper_BUILD_CLIPPER)
	set(explicit_copyright_libpolyclipping
	  "libpolyclipping-${CLIPPER_VERSION}.txt"
	  "${PROJECT_SOURCE_DIR}/3rd-party/clipper/copyright"
	  "3rd-party"
	)
endif()


if(LICENSING_PROVIDER)
	message(STATUS "Licensing information provided by: ${LICENSING_PROVIDER}")
	set(third_party_copyright )
	set(common_licenses )
	unset(package)
	unset(basename)
	include("${LICENSING_PROVIDER}-licensing.cmake")
else()
	set(licensing_warning "
This build is a configuration without an external provider of third-party \
licensing and copyright information. It may not fulfill requirements for \
redistribution of third-party components."
	)
	message(WARNING "${licensing_warning}")
	set(third_party_copyright "<li>${licensing_warning}</li>")
	set(common_licenses )
endif()	



function(add_copyright var name path)
	if(name AND path MATCHES "file:///")
		string(REPLACE "file:///" "/" file "${path}")
		if(NOT EXISTS "${file}")
			message(FATAL_ERROR "No such license file: ${file}")
		endif()
		list(APPEND ${var} "  <li><a href=\"${path}\">${name}</a></li>")
	elseif(name AND path STREQUAL "-" AND ARGN)
		list(APPEND ${var} "  <li>${name}: ${ARGN}</li>")
	elseif(name AND path STREQUAL "-")
		list(APPEND ${var} "  <li>${name}</li>")
	elseif(name AND path AND ARGN)
		set(target ${ARGN})
		string(REPLACE ".txt" "" basename "${name}")
		list(APPEND ${var} "  <li><a href=\"${target}/${name}\">${basename}</a></li>")
		install(FILES "${path}"
		  DESTINATION "${MAPPER_ABOUT_DESTINATION}/${target}"
		  RENAME "${name}"
		)
		# Generate output for manual tests from build dir.
		# As a side effect, make source visible in Qt Creator.
		file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${target}")
		file(GENERATE
		  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}/${name}"
		  INPUT  "${path}"
		)
	elseif()
		message(FATAL_ERROR "Cannot create link for ${name}, ${path}, ${target}")
	endif()
	set(${var} "${${var}}" PARENT_SCOPE)
endfunction()



foreach(entry ${package_names})
	set(entry "explicit_package_${entry}")
	string(REPLACE ":" ";" entry ${entry})
	set(${entry})
endforeach()

list(SORT third_party_components)
if(LICENSING_NO_ATTRIBUTION)
	string(REPLACE "," ";" LICENSING_NO_ATTRIBUTION "${LICENSING_NO_ATTRIBUTION}")
	list(REMOVE_ITEM third_party_components ${LICENSING_NO_ATTRIBUTION})
endif()
list(REMOVE_DUPLICATES third_party_components)
foreach(basename ${third_party_components})
	if(LICENSING_PROVIDER AND NOT DEFINED explicit_copyright_${basename})
		if(explicit_package_${basename})
			set(package ${explicit_package_${basename}})
		else()
			set(package ${basename})
		endif()
		# Exact match
		string(CONFIGURE "${copyright_pattern}" pattern @ONLY)
		file(GLOB path ${pattern})
		if(NOT path)
			# Pattern match
			string(REPLACE "/${package}/" "/${package}*/" pattern "${pattern}")
			file(GLOB raw_path ${pattern})
			set(path )
			foreach(item ${raw_path})
				if(NOT item MATCHES "${package}.*-"
				   OR item MATCHES "${package}[0-9]*-[0-9]")
					list(APPEND path ${item})
				endif()
			endforeach()
		endif()
		if(NOT path)
			message(SEND_ERROR "No license file for ${basename} (${pattern})")
		elseif(path MATCHES ";")
			message(SEND_ERROR "Multiple matches for ${basename}: ${path}")
		elseif(system_copyright_dir AND path MATCHES "^${system_copyright_dir}/")
			set(explicit_copyright_${basename} "${basename}" "file://${path}")
		else()
			string(REGEX REPLACE "-[0-9]*$" "" name ${basename})
			set(explicit_copyright_${basename} "${name}.txt" "${path}" "3rd-party")
		endif()
	endif()
	if(explicit_copyright_${basename})
		add_copyright(third_party_copyright ${explicit_copyright_${basename}})
	endif()
endforeach()

list(SORT common_license_names)
list(REMOVE_DUPLICATES common_license_names)
foreach(basename ${common_license_names})
	if(NOT DEFINED explicit_copyright_${basename})
		string(CONFIGURE "${common_license_pattern}" pattern @ONLY)
		file(GLOB path ${pattern})
		if(NOT path)
			message(SEND_ERROR "No license file for ${basename} (${pattern})")
		elseif(path MATCHES ";")
			message(SEND_ERROR "Multiple matches for ${basename}: ${path}")
		elseif(system_copyright_dir AND path MATCHES "^${system_copyright_dir}/")
			set(explicit_copyright_${basename} "${basename}" "file://${path}")
		else()
			set(explicit_copyright_${basename} "${basename}.txt" "${path}" "common-licenses")
		endif()
	endif()
	if(explicit_copyright_${basename})
		add_copyright(common_licenses ${explicit_copyright_${basename}})
	endif()
endforeach()


string(REPLACE ";" "\n" third_party_copyright "${third_party_copyright}")
string(REPLACE ";" "\n" common_licenses "${common_licenses}")
configure_file(licensing.html "${CMAKE_CURRENT_BINARY_DIR}/licensing.html" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/licensing.html"
  DESTINATION "${MAPPER_ABOUT_DESTINATION}"
)
