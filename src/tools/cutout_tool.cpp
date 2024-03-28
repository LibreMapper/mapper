/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "cutout_tool.h"

#include <functional>

#include <Qt>
#include <QtGlobal>
#include <QCursor>
#include <QFlags>
#include <QKeyEvent>
#include <QPixmap>
#include <QString>

#include "core/map.h"
#include "core/map_coord.h"
#include "core/map_part.h"
#include "core/map_view.h"
#include "core/objects/object.h"
#include "core/symbols/combined_symbol.h"
#include "gui/modifier_key.h"
#include "gui/map/map_editor.h"
#include "gui/map/map_widget.h"
#include "tools/cutout_operation.h"
#include "tools/object_selector.h"
#include "tools/tool.h"
#include "tools/tool_base.h"
#include "util/util.h"


namespace OpenOrienteering {

CutoutTool::CutoutTool(MapEditorController* editor, QAction* tool_action, bool cut_away)
: MapEditorToolBase(QCursor(QPixmap(QString::fromLatin1(":/images/cursor-hollow.png")), 1, 1), Other, editor, tool_action)
, object_selector(new ObjectSelector(map()))
, cut_away(cut_away)
{
	// nothing else
}

CutoutTool::~CutoutTool() = default;


void CutoutTool::finishEditing()
{
	if (editingInProgress())
	{
		if (cutout_object_index >= 0)
			map()->getCurrentPart()->addObject(cutout_object, cutout_object_index);
		map()->clearObjectSelection(false);
		map()->addObjectToSelection(cutout_object, true);
		
		abortEditing();
	}
}

void CutoutTool::initImpl()
{
	// Take cutout shape object out of the map
	Q_ASSERT(map()->getNumSelectedObjects() == 1);
	cutout_object = map()->getFirstSelectedObject()->asPath();
	
	startEditing(cutout_object);
	cutout_object->setSymbol(Map::getCoveringCombinedLine(), true);
	updatePreviewObjects();
	
	cutout_object_index = map()->getCurrentPart()->findObjectIndex(cutout_object);
	map()->removeObjectFromSelection(cutout_object, true);
	map()->releaseObject(cutout_object);
}

void CutoutTool::drawImpl(QPainter* painter, MapWidget* widget)
{
	// Draw selection renderables
	map()->drawSelection(painter, true, widget, nullptr, false);
	
	// Draw cutout shape renderables
	drawSelectionOrPreviewObjects(painter, widget, true);

	// Box selection
	if (isDragging())
		drawSelectionBox(painter, widget, click_pos_map, cur_pos_map);
}

bool CutoutTool::keyPress(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_Return:
		// Insert cutout object again at its original index to keep the objects order
		// (necessary for not breaking undo / redo)
		map()->getCurrentPart()->addObject(cutout_object, cutout_object_index);
		cutout_object_index = -1;
		
		// Apply tool via static function and deselect this tool
		apply(map(), cutout_object, cut_away);
		editor->setEditTool();
		return true;
		
	case Qt::Key_Escape:
		editor->setEditTool();
		return true;
		
	}
	return false;
}

int CutoutTool::updateDirtyRectImpl(QRectF& rect)
{
	rectInclude(rect, cutout_object->getExtent());
	
	map()->includeSelectionRect(rect);
	
	// Box selection
	if (isDragging())
	{
		rectIncludeSafe(rect, click_pos_map);
		rectIncludeSafe(rect, cur_pos_map);
	}
	
	return 0;
}

void CutoutTool::updateStatusText()
{
	QString text;
	if (map()->getNumSelectedObjects() == 0)
	{
		text = tr("<b>%1</b>: Clip the whole map. ").arg(ModifierKey::return_key()) +
		       tr("<b>%1+Click or drag</b>: Select the objects to be clipped. ").arg(ModifierKey::shift());
	}
	else
	{
		text = tr("<b>%1+Click or drag</b>: Select the objects to be clipped. ").arg(ModifierKey::shift()) +
		       tr("<b>%1</b>: Clip the selected objects. ").arg(ModifierKey::return_key());
	}
	text += OpenOrienteering::MapEditorTool::tr("<b>%1</b>: Abort. ").arg(ModifierKey::escape());
	setStatusBarText(text);	
}

void CutoutTool::objectSelectionChangedImpl()
{
	updateDirtyRect();
}

void CutoutTool::clickRelease()
{
	object_selector->selectAt(cur_pos_map, cur_map_widget->getMapView()->pixelToLength(clickTolerance()), active_modifiers & Qt::ShiftModifier);
	updateStatusText();
}

void CutoutTool::dragStart()
{
	// nothing
}

void CutoutTool::dragMove()
{
	updateDirtyRect();
}

void CutoutTool::dragFinish()
{
	object_selector->selectBox(click_pos_map, cur_pos_map, active_modifiers & Qt::ShiftModifier);
	updateStatusText();
}


void CutoutTool::apply(Map* map, PathObject* cutout_object, bool cut_away)
{
	CutoutOperation operation(map, cutout_object, cut_away);
	map->getCurrentPart()->applyOnAllObjects(std::ref(operation));
	operation.commit();
}


}  // namespace OpenOrienteering
