# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2017-2021 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


include("linux-distribution.cmake")

set(system_copyright_dir "/usr/share")
set(copyright_pattern
  "${system_copyright_dir}/doc/@package@/COPYING"
  "${system_copyright_dir}/doc/@package@/COPYRIGHT"
  "${system_copyright_dir}/doc/@package@-libs/LICENSE.TXT" # gdal
  "${system_copyright_dir}/licenses/@package@/COPYING"
  "${system_copyright_dir}/licenses/@package@/Copyright.txt"
  "${system_copyright_dir}/licenses/@package@/LICENSE"
  "${system_copyright_dir}/licenses/@package@-libs/LICENSE.TXT" # gdal
)
