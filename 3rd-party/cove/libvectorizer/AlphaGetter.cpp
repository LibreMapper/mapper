/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#include "AlphaGetter.h"

#include <cmath>
#include <new>

#include "ProgressObserver.h"

namespace cove {
//{@
// \ingroup libvectorizer

/*! \class ClassicAlphaGetter
 * \brief Specialization of KohonenAlphaGetter.
 *
 * It should be used as follows.
 *
 * - Get alpha from getAlpha.
 * - Get E by calling getE.
 * - When alpha is nonzero use it E-times for learning.  Quit otherwise.
 *
 * Several constructors are available, also some setters may be handy.  */

//! Constructs ClassicAlphaGetter with defaults alpha(.1), minAlpha(1e-6),
//! q(.5), E(100000) and no progressObserver.
ClassicAlphaGetter::ClassicAlphaGetter()
	: alpha(.1)
	, minAlpha(1e-6)
	, q(.5)
	, E(100000)
{
}

//! Constructs ClassicAlphaGetter with defaults alpha(.1), minAlpha(1e-6),
//! q(.5), E(100000) and progressObserver.
ClassicAlphaGetter::ClassicAlphaGetter(ProgressObserver* progressObserver)
	: progressObserver(progressObserver)
{
	new (this) ClassicAlphaGetter();
}

//! Constructs ClassicAlphaGetter with given defaults and no progressObserver.
ClassicAlphaGetter::ClassicAlphaGetter(double alpha, double q, unsigned int e,
									   double minAlpha)
	: alpha(alpha)
	, minAlpha(minAlpha)
	, q(q)
	, E(e)
{
}

//! Constructs ClassicAlphaGetter with given defaults and progressObserver.
ClassicAlphaGetter::ClassicAlphaGetter(double alpha, double q, unsigned int e,
									   double minAlpha,
									   ProgressObserver* progressObserver)
	: alpha(alpha)
	, minAlpha(minAlpha)
	, q(q)
	, E(e)
	, progressObserver(progressObserver)
{
}

/*! Returns current value of alpha.
\sa getE */
double ClassicAlphaGetter::getAlpha()
{
	bool cancel = false;
	if (progressObserver)
	{
		progressObserver->setPercentage(
			static_cast<int>(100 * std::log(alpha) / std::log(minAlpha)));
		cancel = progressObserver->isInterruptionRequested();
	}
	double retalpha = (!cancel && alpha > minAlpha) ? alpha : 0;
	alpha *= q;
	return retalpha;
}

/*! Returns current value of E.
\sa getAlpha */
unsigned int ClassicAlphaGetter::getE()
{
	return E;
}

/*! Sets current value of alpha.
\sa getAlpha */
void ClassicAlphaGetter::setAlpha(double alpha)
{
	this->alpha = alpha;
}

/*! Sets minimum value of alpha.  getAlpha returns 0 when alpha is below this
value.
\sa getAlpha */
void ClassicAlphaGetter::setMinAlpha(double minAlpha)
{
	this->minAlpha = minAlpha;
}

/*! Sets multiplication quotient by which alpha gets multiplied.
\sa getAlpha */
void ClassicAlphaGetter::setQ(double q)
{
	this->q = q;
}

/*! Sets how many learning cycles alpha will be used.
\sa getAlpha */
void ClassicAlphaGetter::setE(unsigned int E)
{
	this->E = E;
}
} // cove
//}@
