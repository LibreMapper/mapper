/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "scoped_signals_blocker.h"

#include <QtGlobal>
#include <QObject>


namespace LibreMapper {

ScopedMultiSignalsBlocker::~ScopedMultiSignalsBlocker()
{
#ifdef MAPPER_DEVELOPMENT_BUILD
	// We reserve only 10 items on the stack.
	if (items.size() > 10)
		qWarning("More than 10 items in a ScopedMultiSignalsBlocker");
#endif
	while (!items.empty())
	{
		auto item = items.back();
		if (item.object)
			item.object->blockSignals(item.old_state);
		items.pop_back();
	}
}

void ScopedMultiSignalsBlocker::add(QObject* object)
{
	items.push_back( {object, object && object->blockSignals(true)} );
	Q_ASSERT(!object || object->signalsBlocked());
}


}  // namespace LibreMapper
