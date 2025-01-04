# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2012, 2013, 2014 Thomas Schöps (OpenOrienteering)
# Copyright 2012-2019 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


message(STATUS "Configuring ${PROJECT_NAME} user manual")


set(Mapper_MANUAL_QTHELP_default ON)
if(ANDROID)
	set(Mapper_MANUAL_QTHELP_default OFF)
endif()
option(Mapper_MANUAL_QTHELP
  "Provide the manual as Qt Help collection"
  ${Mapper_MANUAL_QTHELP_default}
)


set(Mapper_MANUAL_PDF_default OFF)
option(Mapper_MANUAL_PDF
  "Provide the manual as PDF file"
  ${Mapper_MANUAL_PDF_default}
)



#
# General target
#
add_custom_target(Mapper-manual ALL)



#
# Creating doxygen input for HTML generation
#

file(GLOB pages "${CMAKE_CURRENT_SOURCE_DIR}/pages/*.md")
configure_file(preprocess-markdown-html.cmake.in preprocess-markdown-html.cmake @ONLY)
add_custom_command(
  OUTPUT    "preprocess-markdown-html.stamp"
  COMMAND   "${CMAKE_COMMAND}" -P preprocess-markdown-html.cmake
  DEPENDS   "preprocess-markdown-html.cmake"
            ${pages}
  COMMENT   "Preprocessing Markdown for HTML output"
)
add_custom_target(Mapper-manual-markdown-html
  DEPENDS   "preprocess-markdown-html.stamp"
  SOURCES   "preprocess-markdown-html.cmake.in"
)



#
# Doxygen HTML generation; creating input for optional Qt Help generation
#

find_program(DOXYGEN_EXECUTABLE
  NAMES doxygen
  DOC "The path of the doxygen executable"
)
if(NOT DOXYGEN_EXECUTABLE)
	message(FATAL_ERROR "doxygen executable not found.")
endif()

set(Mapper_COMPRESSED_HELP "Mapper ${Mapper_VERSION_FILESYSTEM} Manual.qch")
set(Mapper_HELP_COLLECTION "Mapper ${Mapper_VERSION_FILESYSTEM} Manual.qhc")
set(Mapper_HELP_NAMESPACE "openorienteering.mapper-${Mapper_VERSION}.help"
  CACHE INTERNAL "The namespace where the current version's help will be located."
)
if(NOT MANUAL_SECTIONS)
	if(ANDROID)
		set(MANUAL_SECTIONS_DEFAULT ANDROID)
	elseif(APPLE)
		set(MANUAL_SECTIONS_DEFAULT MACOS)
	elseif(UNIX)
		set(MANUAL_SECTIONS_DEFAULT LINUX)
	elseif(WIN32)
		set(MANUAL_SECTIONS_DEFAULT WINDOWS)
	else()
		set(MANUAL_SECTIONS_DEFAULT OTHER)
	endif()
	set(MANUAL_SECTIONS "${MANUAL_SECTIONS_DEFAULT}" CACHE STRING
	  "Conditional manual sections to be enabled")
endif()
message(STATUS "Conditional manual sections: ${MANUAL_SECTIONS}")

set(DOXYFILE_HTML_EXTRA )
if(ANDROID)
	set(DOXYFILE_HTML_EXTRA "
FILE_PATTERNS          = android* toolbar* touch-mode*
USE_MDFILE_AS_MAINPAGE = android-app.md
")
endif()

configure_file(Doxyfile-html.in Doxyfile-html @ONLY)
configure_file(postprocess-qhp.cmake.in postprocess-qhp.cmake @ONLY)
add_custom_command(
  OUTPUT    "html/index.qhp"
  COMMAND   "${CMAKE_COMMAND}" -E remove_directory html
  COMMAND   "${DOXYGEN_EXECUTABLE}" Doxyfile-html
  COMMAND   "${CMAKE_COMMAND}" -P postprocess-qhp.cmake
  DEPENDS   preprocess-markdown-html.stamp
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-html"
            "${CMAKE_CURRENT_BINARY_DIR}/postprocess-qhp.cmake"
            "${DOXYGEN_EXECUTABLE}"
            header.html
            footer.html
            style.css
  COMMENT   "Running doxygen for HTML output"
)
add_custom_target(Mapper-manual-HTML
  DEPENDS   "html/index.qhp"
  # Sources to be listed in Qt Creator
  SOURCES   Doxyfile-html.in
            header.html
            footer.html
            install-html.cmake.in
            postprocess-qhp.cmake.in
            style.css
)
add_dependencies(Mapper-manual-HTML Mapper-manual-markdown-html)



if(Mapper_MANUAL_QTHELP)
	#
	# Qt Help generation
	#
	
	find_package(Qt6 COMPONENTS Core REQUIRED)
	set(qcollectiongenerator_name qhelpgenerator)
	
	# Qt provides a broken Qt5::qcollectiongenerator when crosscompiling.
	if (CMAKE_CROSSCOMPILING AND NOT TARGET Qt5::qcollectiongenerator)
		find_program(Qt5Help_QCOLLECTIONGENERATOR_EXECUTABLE
		  NAMES ${qcollectiongenerator_name}-qt5 ${qcollectiongenerator_name}
		  DOC "The path of the Qt Help collection generator executable"
		)
		qt_add_executable(Qt6::${qcollectiongenerator_name} IMPORTED)
		set_target_properties(Qt6::${qcollectiongenerator_name} PROPERTIES
		  IMPORTED_LOCATION ${Qt5Help_QCOLLECTIONGENERATOR_EXECUTABLE}
		)
	elseif(NOT Qt5Help_QCOLLECTIONGENERATOR_EXECUTABLE)
		find_package(Qt6 COMPONENTS Help REQUIRED)
		set(Qt5Help_QCOLLECTIONGENERATOR_EXECUTABLE Qt5::${qcollectiongenerator_name})
	endif()
	
	# Reproducible builds need a modification to the help collection file.
	set(source_date_commands )
	find_program(SQLITE3_EXECUTABLE NAMES sqlite3
	  DOC "The path of the sqlite3 executable"
	)
	if (DEFINED ENV{SOURCE_DATE_EPOCH} AND SQLITE3_EXECUTABLE)
		file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/timestamp.sql"
		  "UPDATE TimeStampTable SET TimeStamp=strftime('%Y-%m-%dT%H:%M:%S',datetime($ENV{SOURCE_DATE_EPOCH},'unixepoch'));\n"
		)
		set(source_date_commands
	      COMMAND ${SQLITE3_EXECUTABLE} "${Mapper_HELP_COLLECTION}" < "timestamp.sql"
		)
	elseif(DEFINED ENV{SOURCE_DATE_EPOCH})
		message(FATAL_ERROR
		  "By setting SOURCE_DATE_EPOCH, a reproducible build was requested. "
		  "sqlite3 is needed for reproducible builds but cannot be found."
		)
	endif()
	
	configure_file(Manual.qhcp.in Manual.qhcp @ONLY)
	add_custom_command( 
	  OUTPUT    "${Mapper_HELP_COLLECTION}"
	            "${Mapper_COMPRESSED_HELP}"
	  COMMAND   "${Qt6Help_QCOLLECTIONGENERATOR_EXECUTABLE}" Manual.qhcp
	            -o "${Mapper_HELP_COLLECTION}"
	  ${source_date_commands}
	  MAIN_DEPENDENCY "html/index.qhp"
	  DEPENDS   "${CMAKE_CURRENT_BINARY_DIR}/Manual.qhcp"
	  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	  COMMENT   "Running qcollectiongenerator for Qt Help output"
	)
	add_custom_target(Mapper-manual-Qt-Help
	  DEPENDS   "${Mapper_HELP_COLLECTION}"
	  SOURCES   Manual.qhcp.in
	)
	add_dependencies(Mapper-manual-Qt-Help  Mapper-manual-HTML)
	add_dependencies(Mapper-manual  Mapper-manual-Qt-Help)
	
	install(
	  FILES "${CMAKE_CURRENT_BINARY_DIR}/${Mapper_HELP_COLLECTION}" 
	        "${CMAKE_CURRENT_BINARY_DIR}/${Mapper_COMPRESSED_HELP}" 
	  DESTINATION "${MAPPER_ABOUT_DESTINATION}"
	)
else()
	#
	# Direct installation of manual HTML files
	#
	
	add_dependencies(Mapper-manual Mapper-manual-HTML)
	
	configure_file(install-html.cmake.in install-html.cmake @ONLY)
	install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/install-html.cmake")
endif()


if(Mapper_MANUAL_PDF)
	#
	# PDF file generation
	#
	
	find_program(PDFLATEX_EXECUTABLE
	  NAMES pdflatex
	  DOC "The path of the pdflatex executable"
	)
	if(PDFLATEX_EXECUTABLE)
		find_program(MAKE_EXECUTABLE
		  NAMES make
		  DOC "The path of the make executable"
		)
		if(NOT MAKE_EXECUTABLE AND CMAKE_HOST_WIN32)
			set(MAKE_EXECUTABLE make.bat)
		endif()
	endif()
	
	set(Mapper_PDF_manual "OpenOrienteering-Mapper-${Mapper_VERSION_FILESYSTEM}-Manual.pdf")
	
	configure_file(Doxyfile-pdflatex.in Doxyfile-pdflatex @ONLY)
	configure_file(preprocess-markdown-pdflatex.cmake.in preprocess-markdown-pdflatex.cmake @ONLY)
	configure_file(postprocess-pdflatex.cmake.in postprocess-pdflatex.cmake @ONLY)
	add_custom_command(
	  OUTPUT    "preprocess-markdown-pdflatex.stamp"
	            "Doxyfile-pdflatex-input.txt"
	  COMMAND   "${CMAKE_COMMAND}" -P preprocess-markdown-pdflatex.cmake
	  DEPENDS   "${CMAKE_CURRENT_BINARY_DIR}/preprocess-markdown-pdflatex.cmake"
	            ${pages}
	  COMMENT   "Preprocessing Markdown for pdflatex output"
	)
	add_custom_command( 
	  OUTPUT    "${Mapper_PDF_manual}"
	  COMMAND   "${DOXYGEN_EXECUTABLE}" Doxyfile-pdflatex
	  COMMAND   "${CMAKE_COMMAND}" -P postprocess-pdflatex.cmake
	  COMMAND   "${MAKE_EXECUTABLE}" -C pdflatex
	  COMMAND   "${CMAKE_COMMAND}" -E copy_if_different "pdflatex/refman.pdf" "${Mapper_PDF_manual}"
	  COMMAND   "${CMAKE_COMMAND}" -E remove "pdflatex/refman.pdf"
	  MAIN_DEPENDENCY preprocess-markdown-pdflatex.stamp
	  DEPENDS   postprocess-pdflatex.cmake
	  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	  COMMENT   "Running pdflatex for PDF output"
	)
	add_custom_target(Mapper-manual-PDF
	  DEPENDS   "${Mapper_PDF_manual}"
	  SOURCES   Doxyfile-pdflatex.in
	            postprocess-pdflatex.cmake.in
	)
	add_dependencies(Mapper-manual Mapper-manual-PDF)
	
	install(
	  FILES "${CMAKE_CURRENT_BINARY_DIR}/${Mapper_PDF_manual}" 
	  DESTINATION "${MAPPER_ABOUT_DESTINATION}"
	)
endif()

