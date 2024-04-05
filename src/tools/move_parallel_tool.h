/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_MOVE_PARALLEL_TOOL_H
#define LIBREMAPPER_MOVE_PARALLEL_TOOL_H

#include <QObject>
#include <QString>
#include <QtGlobal>

#include <memory>

#include "core/map_coord.h"
#include "tools/edit_tool.h"
#include "tools/tool_base.h"

class QAction;
class QPainter;


namespace LibreMapper {

class MapEditorController;
class MapRenderables;
class MapWidget;
class PathObject;

/**
 * A tool to edit lines of PathObjects.
 */
class MoveParallelTool : public MapEditorToolBase
{
	using HoverFlag = EditTool::HoverFlag;
	using HoverState = EditTool::HoverState;
	
Q_OBJECT
public:
	MoveParallelTool(MapEditorController* editor, QAction* tool_action);
	~MoveParallelTool();

protected:
	void mouseMove() override;
	void dragStart() override;
	void dragMove() override;
	void dragFinish() override;
	void drawImpl(QPainter* painter, MapWidget* widget) override;
	
	void updateHoverState();
	
	void updateStatusText() override;
	void objectSelectionChangedImpl() override;
	
private:
	/**
	 * An object created for the current hover_line.
	 */
	std::unique_ptr<PathObject> highlight_object;
	std::unique_ptr<MapRenderables> highlight_renderables;

	/**
	 * Provides general information on what is hovered over.
	 */
	HoverState hover_state = HoverFlag::OverNothing;

	/**
	 * Object which is hovered over (if any).
	 */
	PathObject* hover_object = {};

	/**
	 * A copy of the original object which we shift. The new object is
	 * always a direct modification of the initial copy, so that we
	 * don't accumulate errors.
	 */
	std::unique_ptr<PathObject> orig_hover_object;

	/**
	 * Closest point on the dragged path. We use this point to calculate
	 * the object shift distance.
	 */
	MapCoordF path_drag_point;

	/**
	 * Normal vector at the closest point of the highlighted path. We use the
	 * normal vector for distance calsulation in dragMove().
	 */
	MapCoordF path_normal_vector;

	/**
	  * The move distance is exposed from dragMove() for the purpose of
	  * status text updates.
	  */
	qreal move_distance = {};
};


}  // namespace LibreMapper

#endif
