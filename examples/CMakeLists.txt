# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2013-2017 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.
 
set(Mapper_EXAMPLES
  overprinting.omap
  forest\ sample.omap
  complete\ map.omap
)

if(ANDROID)
	# \todo Support overprinting simulation in the mobile app.
	list(REMOVE_ITEM Mapper_EXAMPLES overprinting.omap)
endif()

install(
  FILES ${Mapper_EXAMPLES}
  DESTINATION "${MAPPER_DATA_DESTINATION}/examples"
)

foreach(_example ${Mapper_EXAMPLES} examples.qrc)
	configure_file(${_example} ${_example} COPYONLY)
endforeach()

if(Mapper_DEVELOPMENT_BUILD)
	configure_file(overprinting.omap autosave-test.omap COPYONLY)
	configure_file(complete\ map.omap autosave-test.omap.autosave COPYONLY)
endif()
