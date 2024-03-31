/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2015, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "template_tool_move.h"

#include <Qt>
#include <QCursor>
#include <QFlags>
#include <QMouseEvent>

#include "core/map.h"
#include "templates/template.h"
#include "util/transformation.h"


namespace LibreMapper {

TemplateMoveTool::TemplateMoveTool(Template* templ, MapEditorController* editor, QAction* tool_action)
: MapEditorTool(editor, Other, tool_action), templ(templ)
{
	// nothing else
}

TemplateMoveTool::~TemplateMoveTool() = default;



void TemplateMoveTool::init()
{
	setStatusBarText(tr("<b>Drag</b> to move the current template"));
	
	connect(map(), &Map::templateDeleted, this, &TemplateMoveTool::templateDeleted);
	
	MapEditorTool::init();
}

const QCursor& TemplateMoveTool::getCursor() const
{
	static auto const cursor = QCursor(Qt::SizeAllCursor);
	return cursor;
}

bool TemplateMoveTool::mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget)
{
	Q_UNUSED(widget);
	
	if (event->button() != Qt::LeftButton)
		return false;
	
	dragging = true;
	click_pos_map = map_coord;
	return true;
}
bool TemplateMoveTool::mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget)
{
	Q_UNUSED(widget);
	
	if (!(event->buttons() & Qt::LeftButton) || !dragging)
		return false;
	
	updateDragging(map_coord);
	return true;
}
bool TemplateMoveTool::mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget)
{
	Q_UNUSED(widget);
	
	if (event->button() != Qt::LeftButton || !dragging)
		return false;
	
	updateDragging(map_coord);
	dragging = false;
	return true;
}

void TemplateMoveTool::templateDeleted(int index, const Template* temp)
{
	Q_UNUSED(index);
	if (templ == temp)
		deactivate();
}

void TemplateMoveTool::updateDragging(const MapCoordF& mouse_pos_map)
{
	auto move = MapCoord { mouse_pos_map - click_pos_map };
	click_pos_map = mouse_pos_map;
	
	templ->setTemplateAreaDirty();
	templ->setTemplatePosition(templ->templatePosition() + move);
	templ->setTemplateAreaDirty();
	
	for (int i = 0; i < templ->getNumPassPoints(); ++i)
	{
		PassPoint* point = templ->getPassPoint(i);
		auto move_f = MapCoordF { move };
		
		if (templ->isAdjustmentApplied())
		{
			point->dest_coords += move_f;
			point->calculated_coords += move_f;
		}
		else
		{
			point->src_coords += move_f;
		}
	}
	
	map()->emitTemplateChanged(templ);
}


}  // namespace LibreMapper
