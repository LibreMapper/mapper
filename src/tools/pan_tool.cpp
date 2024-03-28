/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "pan_tool.h"

#include <Qt>
#include <QCursor>
#include <QPoint>

#include "core/map_view.h"
#include "gui/map/map_widget.h"
#include "tools/tool.h"


namespace OpenOrienteering {

PanTool::PanTool(MapEditorController* editor, QAction* tool_action)
 : MapEditorToolBase(QCursor(Qt::OpenHandCursor), Pan, editor, tool_action)
{
	useTouchCursor(false);
}

PanTool::~PanTool() = default;



void PanTool::clickPress()
{
	startDragging();
}

void PanTool::dragStart()
{
	cur_map_widget->setCursor(Qt::ClosedHandCursor);
}

void PanTool::dragMove()
{
	MapView* view = mapWidget()->getMapView();
	view->setPanOffset(cur_pos - click_pos);
}

void PanTool::dragFinish()
{
	MapView* view = mapWidget()->getMapView();
	view->finishPanning(cur_pos - click_pos);
	
	cur_map_widget->setCursor(Qt::OpenHandCursor);
}

void PanTool::dragCanceled()
{
	// finalize current panning
	MapView* view = mapWidget()->getMapView();
	view->finishPanning(view->panOffset());
	
	cur_map_widget->setCursor(Qt::OpenHandCursor);
}

void PanTool::updateStatusText()
{
	setStatusBarText(tr("<b>Drag</b>: Move the map. "));
}

void PanTool::objectSelectionChangedImpl()
{
	// Nothing
}


}  // namespace OpenOrienteering
