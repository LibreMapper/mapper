# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2017 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


# Based on Debian testing as of 2017-04.
# Also used for Ubuntu.

include("linux-distribution.cmake")

# A 3rd-party Mapper dependency may consist of multiple DEB packages
# sharing the same copyright file. We reference it just once.
set(package_names
  gdal:libgdal
)

set(system_copyright_dir   "/usr/share")
set(copyright_pattern      "${system_copyright_dir}/doc/@package@/copyright")
set(common_license_pattern "${system_copyright_dir}/common-licenses/@basename@")
