/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor
 *
 * This file is part of CoVe.
 */

#include "Concurrency.h"


namespace cove {

// ### ProgressObserver, from ProgressObserver.h ###

ProgressObserver::~ProgressObserver() = default;



namespace Concurrency {

// ### Concurrency::Progress ###

static_assert(std::is_copy_constructible<Progress>::value, "Concurrency::Progress must be copy constructible.");
static_assert(std::is_move_constructible<Progress>::value, "Concurrency::Progress must be move constructible.");
static_assert(!std::is_copy_assignable<Progress>::value, "Concurrency::Progress is not copy assignable.");
static_assert(!std::is_move_assignable<Progress>::value, "Concurrency::Progress is not move assignable.");

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


// ### Concurrency::Job ###

using ArbitraryResultType = int;
static_assert(std::is_copy_constructible<Job<ArbitraryResultType>>::value, "Concurrency::Job must be copy constructible.");
static_assert(std::is_move_constructible<Job<ArbitraryResultType>>::value, "Concurrency::Job must be move constructible.");
static_assert(!std::is_copy_assignable<Job<ArbitraryResultType>>::value, "Concurrency::Job is not copy assignable.");
static_assert(!std::is_move_assignable<Job<ArbitraryResultType>>::value, "Concurrency::Job is not move assignable.");


}  // namespace Concurrency

}  // cove

