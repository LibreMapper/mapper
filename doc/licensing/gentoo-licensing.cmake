# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2019 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


include("linux-distribution.cmake")

set(system_copyright_dir "/usr/share")
# Pattern to find GDAL license.
set(copyright_pattern
  "${system_copyright_dir}/@package@/LICENSE.TXT"
)

# Location of common license text:
set(common_license_pattern "/usr/portage/licenses/@basename@")
