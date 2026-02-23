/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2025 Andreas Bertheussen (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "box_zoom_tool.h"

#include <Qt>
#include <QCursor>
#include <QKeyEvent>
#include <QPixmap>
#include <QRectF>

#include "core/map_coord.h"
#include "gui/main_window.h"
#include "gui/map/map_widget.h"
#include "tools/tool.h"
#include "util/util.h"

namespace LibreMapper {

BoxZoomTool::BoxZoomTool(MapEditorController* editor, QAction* tool_action)
 : MapEditorToolBase( scaledToScreen(QCursor{ QPixmap(QString::fromLatin1(":/images/cursor-zoom.png")), 12, 12 }), BoxZoom, editor, tool_action)
{
	useTouchCursor(false);
}

BoxZoomTool::~BoxZoomTool() = default;

void BoxZoomTool::clickPress()
{
	startDragging();
}

bool BoxZoomTool::keyPress(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape && isDragging())
	{
		cancelDragging();
		return true;
	}
	return false;
}

void BoxZoomTool::dragStart()
{
	mainWindow()->clearStatusBarMessage();
	setEditingInProgress(true);
	updateDirtyRect();
}

void BoxZoomTool::dragMove()
{
	updateDirtyRect();
}

void BoxZoomTool::dragFinish()
{
	constexpr auto min_size = 1; // mm
	auto const rect = QRectF(click_pos_map, cur_pos_map).normalized();
	if (rect.width() >= min_size && rect.height() >= min_size)
		mapWidget()->adjustViewToRect(rect, MapWidget::ContinuousZoom);
	else
		mainWindow()->showStatusBarMessage(tr("The selected box is too small."), 2000);

	updateDirtyRect();
	setEditingInProgress(false);
}

void BoxZoomTool::dragCanceled()
{
	updateDirtyRect();
	setEditingInProgress(false);
}

void BoxZoomTool::updateStatusText()
{
	setStatusBarText(tr("<b>Drag</b>: Select area to zoom in on. "));
}

void BoxZoomTool::objectSelectionChangedImpl()
{}

void BoxZoomTool::drawImpl(QPainter* painter, MapWidget* widget)
{
	drawSelectionOrPreviewObjects(painter, widget, false);

	if (isDragging())
		drawSelectionBox(painter, widget, click_pos_map, cur_pos_map);
}

int BoxZoomTool::updateDirtyRectImpl(QRectF& rect)
{
	rectIncludeSafe(rect, click_pos_map);
	rectInclude(rect, cur_pos_map);
	return 5;
}

}  // namespace LibreMapper
