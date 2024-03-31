/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_SEGMENTED_BUTTON_LAYOUT_H
#define LIBREMAPPER_SEGMENTED_BUTTON_LAYOUT_H

#include <QtGlobal>
#include <QObject>
#include <QHBoxLayout>

class QWidget;

namespace LibreMapper {


/**
 * SegmentedButtonLayout is a horizontal box layout with no margin and no
 * spacing which will mark the contained widgets as being segments having a
 * left and/or right neighbor.
 * 
 * MapperProxyStyle uses this information to make buttons from a single
 * SegmentedButtonLayout appear as a single segmented button.
 */
class SegmentedButtonLayout : public QHBoxLayout
{
Q_OBJECT
public:
	/**
	 * Constructs a new SegmentedButtonLayout.
	 */
	SegmentedButtonLayout();
	
	/**
	 * Constructs a new SegmentedButtonLayout for the given parent.
	 */
	explicit SegmentedButtonLayout(QWidget* parent);
	
	/**
	 * Destroys the object.
	 */
	~SegmentedButtonLayout() override;
	
	/**
	 * Resets the information about neighboring segments and any other cached
	 * information about the layout.
	 */
	void invalidate() override;
	
	/**
	 * Types of segment neighborhood.
	 */
	enum Segment
	{
		NoNeighbors   = 0x00,
		RightNeighbor = 0x01,
		LeftNeighbor  = 0x02,
		BothNeighbors = RightNeighbor | LeftNeighbor
	};
	
private:
	Q_DISABLE_COPY(SegmentedButtonLayout)
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_SEGMENTED_BUTTON_LAYOUT_H
