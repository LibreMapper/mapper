# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2017-2022 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


# Based on Arch Extra as of 2017-04.

include("linux-distribution.cmake")

set(system_copyright_dir "/usr/share/licenses")
set(copyright_pattern
  "${system_copyright_dir}/@package@/LICENSE"
  "${system_copyright_dir}/@package@/LICENSE.TXT" # gdal
)
