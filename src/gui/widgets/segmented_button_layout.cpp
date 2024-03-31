/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "segmented_button_layout.h"

#include <QLayoutItem>
#include <QVariant>  // IWYU pragma: keep
#include <QWidget>


namespace LibreMapper {

SegmentedButtonLayout::SegmentedButtonLayout()
 : QHBoxLayout()
{
	setContentsMargins(0, 0, 0, 0);
	setSpacing(0);
}

SegmentedButtonLayout::SegmentedButtonLayout(QWidget* parent)
 : QHBoxLayout(parent)
{
	setContentsMargins(0, 0, 0, 0);
	setSpacing(0);
}

SegmentedButtonLayout::~SegmentedButtonLayout()
{
	; // Nothing, not inlined
}

void SegmentedButtonLayout::invalidate()
{
	int num_items = count();
	QWidget* first_widget = nullptr;
	QWidget* widget = nullptr;
	for (int i = 0; i < num_items; ++i)
	{
		widget = itemAt(i)->widget();
		if (!widget)
			continue;
		
// 		widget->setStyle(segmented_style);
		
		if (!first_widget)
		{
			first_widget = widget;
			widget->setProperty("segment", RightNeighbor);
		}
		else
		{
			widget->setProperty("segment", BothNeighbors);
		}
	}
	
	if (widget)
		widget->setProperty("segment", (widget == first_widget) ? NoNeighbors : LeftNeighbor);
	
	QHBoxLayout::invalidate();
}


}  // namespace LibreMapper
