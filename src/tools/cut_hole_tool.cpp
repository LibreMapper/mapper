/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "cut_hole_tool.h"

#include <iterator>
#include <map>
#include <memory>

#include <Qt>
#include <QtGlobal>
#include <QCursor>
#include <QFlags>
#include <QMouseEvent>
#include <QPixmap>
#include <QString>

#include "core/map.h"
#include "core/objects/boolean_tool.h"
#include "core/objects/object.h"
#include "core/symbols/symbol.h"
#include "tools/draw_circle_tool.h"
#include "tools/draw_line_and_area_tool.h"
#include "tools/draw_path_tool.h"
#include "tools/draw_rectangle_tool.h"
#include "tools/tool.h"
#include "undo/object_undo.h"
#include "undo/undo.h"


namespace OpenOrienteering {

CutHoleTool::CutHoleTool(MapEditorController* editor, QAction* tool_action, CutHoleTool::HoleType hole_type)
: MapEditorTool(editor, Other, tool_action)
, hole_type(hole_type)
{}

void CutHoleTool::init()
{
	connect(map(), &Map::objectSelectionChanged, this, &CutHoleTool::objectSelectionChanged);
	updateDirtyRect();
	updateStatusText();
	
	MapEditorTool::init();
}

const QCursor& CutHoleTool::getCursor() const
{
	static auto const cursor = scaledToScreen(QCursor{ QPixmap(QString::fromLatin1(":/images/cursor-cut.png")), 11, 11 });
	return cursor;
}

void CutHoleTool::finishEditing()
{
	Q_ASSERT(editingInProgress());
	if (path_tool)
		path_tool->finishEditing();
	MapEditorTool::finishEditing();
}

CutHoleTool::~CutHoleTool()
{
	delete path_tool;
}

bool CutHoleTool::mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget)
{
	if (path_tool)
		return path_tool->mousePressEvent(event, map_coord, widget);
	
	if (!(event->buttons() & Qt::LeftButton))
		return false;
	
	// Start a new hole
	switch (hole_type)
	{
	case CutHoleTool::Path:
		path_tool = new DrawPathTool(editor, nullptr, true, true);
		break;
	case CutHoleTool::Circle:
		path_tool = new DrawCircleTool(editor, nullptr, true);
		break;
	case CutHoleTool::Rect:
		path_tool = new DrawRectangleTool(editor, nullptr, true);
		break;
	/* no default; watch compiler warnings for unhandled cases! */
	}
	
	connect(path_tool, &DrawLineAndAreaTool::dirtyRectChanged, this, &CutHoleTool::pathDirtyRectChanged);
	connect(path_tool, &DrawLineAndAreaTool::pathAborted, this, &CutHoleTool::pathAborted);
	connect(path_tool, &DrawLineAndAreaTool::pathFinished, this, &CutHoleTool::pathFinished);
	
	path_tool->init();
	path_tool->mousePressEvent(event, map_coord, widget);
	Q_ASSERT(path_tool->editingInProgress());
	setEditingInProgress(true);
	
	return true;
}

bool CutHoleTool::mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget)
{
	if (path_tool)
		return path_tool->mouseMoveEvent(event, map_coord, widget);
	
	return false;
}

bool CutHoleTool::mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget)
{
	if (path_tool)
		return path_tool->mouseReleaseEvent(event, map_coord, widget);
	
	return false;
}

bool CutHoleTool::mouseDoubleClickEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget)
{
	if (path_tool)
		return path_tool->mouseDoubleClickEvent(event, map_coord, widget);
	return false;
}

void CutHoleTool::leaveEvent(QEvent* event)
{
	if (path_tool)
		path_tool->leaveEvent(event);
}

bool CutHoleTool::keyPressEvent(QKeyEvent* event)
{
	if (path_tool)
		return path_tool->keyPressEvent(event);
	return false;
}

bool CutHoleTool::keyReleaseEvent(QKeyEvent* event)
{
	if (path_tool)
		return path_tool->keyReleaseEvent(event);
	return false;
}

void CutHoleTool::focusOutEvent(QFocusEvent* event)
{
	if (path_tool)
		path_tool->focusOutEvent(event);
}

void CutHoleTool::draw(QPainter* painter, MapWidget* widget)
{
	map()->drawSelection(painter, true, widget, nullptr);
	
	if (path_tool)
		path_tool->draw(painter, widget);
}

void CutHoleTool::updateDirtyRect(const QRectF* path_rect)
{
	QRectF rect;
	if (path_rect)
		rect = *path_rect;
	map()->includeSelectionRect(rect);
	
	if (rect.isValid())
		map()->setDrawingBoundingBox(rect, 6, true);
	else
		map()->clearDrawingBoundingBox();
}

void CutHoleTool::objectSelectionChanged()
{
	Map* map = this->map();
	if (map->getNumSelectedObjects() != 1 || !((*map->selectedObjectsBegin())->getSymbol()->getContainedTypes() & Symbol::Area))
		deactivate();
	else
		updateDirtyRect();
}

void CutHoleTool::pathDirtyRectChanged(const QRectF& rect)
{
	updateDirtyRect(&rect);
}

void CutHoleTool::pathAborted()
{
	path_tool->deleteLater();
	path_tool = nullptr;
	setEditingInProgress(false);
	updateDirtyRect();
	updateStatusText();
}

void CutHoleTool::pathFinished(PathObject* hole_path)
{
	if (map()->getNumSelectedObjects() > 0)
	{
		auto* edited_object = *begin(map()->selectedObjects());
		if (edited_object->getType() == Object::Path)
		{
			cutHole(static_cast<PathObject*>(edited_object), hole_path);
			return;
		}
	}
	pathAborted();
}
	
void CutHoleTool::cutHole(PathObject* edited_path, PathObject* hole_path)
{
	// Close the hole path
	Q_ASSERT(hole_path->parts().size() == 1);
	hole_path->parts().front().setClosed(true, true);
	
	BooleanTool::PathObjects out_objects;
	out_objects.reserve(1);
	
	BooleanTool tool(BooleanTool::Difference, map());
	if (!tool.executeForObjects(edited_path, { hole_path }, out_objects))
		return;
	
	auto* undo_step = new CombinedUndoStep(map());
	
	auto* add_step = new AddObjectsUndoStep(map());
	add_step->addObject(edited_path, edited_path);
	map()->removeObjectFromSelection(edited_path, false);
	map()->releaseObject(edited_path);
	edited_path = nullptr;
	undo_step->push(add_step);
	
	auto* delete_step = new DeleteObjectsUndoStep(map());
	for (auto* object : out_objects)
	{
		auto index = map()->addObject(object);
		map()->addObjectToSelection(object, false);
		delete_step->addObject(index);
	}
	undo_step->push(delete_step);
	
	map()->push(undo_step);
	
	updateDirtyRect();
	map()->setObjectsDirty();
	
	// Don't signal selection change before editing was ended with pathAborted.
	pathAborted();
	map()->emitSelectionChanged();
}

void CutHoleTool::updateStatusText()
{
	if (!path_tool)
	{
		// FIXME: The path_tool would have better instruction, but is not initialized yet.
		setStatusBarText(tr("<b>Click or drag</b>: Start drawing the hole. "));
	}
}


}  // namespace OpenOrienteering
