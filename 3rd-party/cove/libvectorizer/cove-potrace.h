/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019 Kai Pastor
 *
 * This file is part of CoVe. 
 */

#ifndef COVE_POTRACE_H
#define COVE_POTRACE_H

/**
 * \file
 * A sane C++ header for potrace.
 * 
 * Potrace's (internal) headers used by CoVe export some macros which interfere
 * with C++ function names. This C++ header is to include all potrace C headers
 * needed for Cove, and undefine these macros.
 */

extern "C" {

// IWYU pragma: begin_exports
#include "potrace/potracelib.h"
#include "potrace/auxiliary.h"
#include "potrace/curve.h"
#include "potrace/lists.h"
#include "potrace/trace.h"
// IWYU pragma: end_exports

}


// Get rid of the the macros from potrace/auxiliary.h.

#ifdef abs
#  undef abs
#endif

#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif


#endif  // COVE_POTRACE_H
