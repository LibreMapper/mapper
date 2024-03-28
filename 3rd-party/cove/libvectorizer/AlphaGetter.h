/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_ALPHAGETTER_H
#define COVE_ALPHAGETTER_H

#include "KohonenMap.h"

namespace cove {
class ProgressObserver;

class ClassicAlphaGetter : public KohonenAlphaGetter
{
	double alpha, minAlpha, q;
	unsigned int E;
	ProgressObserver* progressObserver;

public:
	ClassicAlphaGetter();
	ClassicAlphaGetter(ProgressObserver* progressObserver);
	ClassicAlphaGetter(double alpha, double q, unsigned int e, double minAlpha);
	ClassicAlphaGetter(double alpha, double q, unsigned int e, double minAlpha,
	                   ProgressObserver* progressObserver);
	double getAlpha() override;
	unsigned int getE() override;

	virtual void setAlpha(double alpha);
	virtual void setMinAlpha(double minAlpha);
	virtual void setQ(double alpha);
	virtual void setE(unsigned int alpha);
};
} // cove

#endif
