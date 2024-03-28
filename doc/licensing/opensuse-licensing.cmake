# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2017, 2018 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.

# Based on openSUSE Leap 42.3.

include("linux-distribution.cmake")

find_file (GDAL_LICENSE_TXT
  NAMES LICENSE.TXT
  PATHS /usr/share/licenses/gdal-devel
        /usr/share/doc/packages/gdal-devel
  NO_DEFAULT_PATH
)
set(explicit_copyright_gdal
  # Copy! Package libgdal* does not include this file.
  "gdal.txt"
  "${GDAL_LICENSE_TXT}"
  "3rd-party"
)

set(system_copyright_dir "/usr/share/doc/packages")
set(copyright_pattern
  "${system_copyright_dir}/@package@/COPYING"
  "${system_copyright_dir}/@package@/COPYRIGHT"
  "${system_copyright_dir}/@package@/Copyright.txt"
  "${system_copyright_dir}/@package@/LICENSE"
)
