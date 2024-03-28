/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_CUT_TOOL_H
#define OPENORIENTEERING_CUT_TOOL_H

#include <vector>

#include <QObject>
#include <QRectF>
#include <QScopedPointer>
#include <QString>

#include "core/map_coord.h"
#include "core/path_coord.h"
#include "core/objects/object.h"
#include "tools/edit_tool.h"
#include "tools/tool_base.h"

class QAction;
class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QRectF;

namespace OpenOrienteering {

class DrawPathTool;
class MapEditorController;
class MapRenderables;
class MapWidget;


/**
 * A tool to cut (split) lines and areas into smaller pieces.
 */
class CutTool : public MapEditorToolBase
{
Q_OBJECT
public:
	using length_type = PathCoord::length_type;
	using HoverFlag   = EditTool::HoverFlag;
	using HoverState  = EditTool::HoverState;
	
	CutTool(MapEditorController* editor, QAction* tool_action);
	~CutTool() override;
	
protected:
	void initImpl() override;
	
	void updateStatusText() override;
	
	void objectSelectionChangedImpl() override;
	
	// MapEditorTool input event handlers
	bool mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseDoubleClickEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	void leaveEvent(QEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;
	
	// MapEditorToolBase input event handlers
	void mouseMove() override;
	void clickPress() override;
	void clickRelease() override;
	void dragStart() override;
	void dragMove() override;
	void dragFinish() override;
	bool keyPress(QKeyEvent* event) override;
	bool keyRelease(QKeyEvent* event) override;
	
	// Functions for splitting lines
	void startCuttingLine(const ObjectPathCoord& point);
	void updateCuttingLine(const MapCoordF& cursor_pos);
	void finishCuttingLine();
	
	// Functions for splitting areas
	bool startCuttingArea(const ObjectPathCoord& point);
	void abortCuttingArea();
	void finishCuttingArea(PathObject* split_path);
	
	// Drawing
	void updatePreviewObjects() override;
	void deletePreviewObject();
	int updateDirtyRectImpl(QRectF& rect) override;
	void drawImpl(QPainter* painter, MapWidget* widget) override;
	
	// State 
	void updateHoverState(const MapCoordF& cursor_pos);
	ObjectPathCoord findEditPoint(const MapCoordF& cursor_pos_map, int with_type, int without_type) const;
	
	/**
	 * Replaces the given object in the map with the replacement objects.
	 * 
	 * Creates the necessary undo steps. If replacement is empty, the object is
	 * deleted without replacement.
	 * 
	 * @todo Consider moving this to a more general class (Map, MapPart).
	 */
	void replaceObject(Object* object, const std::vector<PathObject*>& replacement) const;
	
	// Basic state
	bool waiting_for_mouse_release = false;
	HoverState  hover_state = HoverFlag::OverNothing;
	PathObject* hover_object = nullptr;
	MapCoordVector::size_type hover_point = 0;
	
	// The object which is about to be split
	PathObject* edit_object = nullptr;
	
	// State for splitting lines
	PathPartVector::size_type drag_part_index;
	length_type drag_start_len;
	length_type drag_end_len;
	bool reverse_drag; // if true, the effective drag range is [drag_end_len; drag_start_len]
	
	// State for cutting areas
	DrawPathTool* path_tool = nullptr;
	QRectF path_tool_rect;
	
	// Preview objects for dragging
	PathObject* preview_path = nullptr;
	/**
	 * The renderables member in MapEditorToolBase contains the selection at the moment,
	 * but the path of cutting a line needs to be drawn separately.
	 * \todo Rewrite renderables handling in MapEditorToolBase so that we can remove it here.
	 */
	QScopedPointer<MapRenderables> renderables;
};


}  // namespace OpenOrienteering

#endif
