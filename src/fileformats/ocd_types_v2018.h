/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2018 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_OCD_TYPES_V2018
#define OPENORIENTEERING_OCD_TYPES_V2018

#include "ocd_types.h"
#include "ocd_types_v12.h"

namespace Ocd
{
	/*
	 * There is no documentation for version 2018 of the OCD file format.
	 * It seems reasonable to assume only marginal changes from the version 12
	 * format:
	 * - Program features added in version 2018 do not seem to require mandatory
	 *   modifications to the file format.
	 * - Sample maps from the free version 2018 Viewer could be successfully
	 *   loaded by the free version 12 Viewer, after changing the file format
	 *   version fields in the file header.
	 */
	using FormatV2018 = FormatV12;
	
}

#endif // OPENORIENTEERING_OCD_TYPES_V2018_H
