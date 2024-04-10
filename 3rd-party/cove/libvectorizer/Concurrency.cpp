/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor
 *
 * This file is part of CoVe.
 */

#include "Concurrency.h"

#include <memory>

#include <QAtomicInteger>


namespace cove {

// ### ProgressObserver, from ProgressObserver.h ###

ProgressObserver::~ProgressObserver() = default;



namespace Concurrency {

// ### Concurrency::Progress ###

int Progress::getPercentage() const noexcept
{
	return data->percentage.loadRelaxed();
}

void Progress::setPercentage(int percentage)
{
	data->percentage.storeRelaxed(percentage);
}

bool Progress::isInterruptionRequested() const
{
	return bool(data->canceled.loadRelaxed());
}

void Progress::requestInterruption() noexcept
{
	data->canceled.storeRelaxed(int(true));
}


// ### Concurrency::TransformedProgress ###

void TransformedProgress::setPercentage(int percentage)
{
	observer.setPercentage(qBound(0, qRound(offset + factor * percentage), 100));
}


}  // namespace Concurrency

}  // cove

