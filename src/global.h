/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_GLOBAL_H
#define OPENORIENTEERING_GLOBAL_H

namespace OpenOrienteering {

/** 
 * This is called at startup in main() and by the test cases
 * to do the global initializations.
 */
void doStaticInitializations();


}  // namespace OpenOrienteering

#endif
