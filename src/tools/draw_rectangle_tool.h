/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014, 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_DRAW_RECTANGLE_H
#define LIBREMAPPER_DRAW_RECTANGLE_H

#include <vector>

#include <QObject>
#include <QPoint>
#include <QString>

#include "core/map_coord.h"
#include "tools/draw_line_and_area_tool.h"

#include <QPointer>
#include <QScopedPointer>

class QAction;
class QCursor;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QToolButton;

namespace LibreMapper {

class ConstrainAngleToolHelper;
class KeyButtonBar;
class MapEditorController;
class MapWidget;
class SnappingToolHelper;


/**
 * Tool to draw rectangular PathObjects (but also 45 degree angles).
 */
class DrawRectangleTool : public DrawLineAndAreaTool
{
Q_OBJECT
public:
	DrawRectangleTool(MapEditorController* editor, QAction* tool_action, bool is_helper_tool);
    ~DrawRectangleTool() override;
	
	void init() override;
	const QCursor& getCursor() const override;
	
	bool mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseDoubleClickEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
	bool keyPressEvent(QKeyEvent* event) override;
    bool keyReleaseEvent(QKeyEvent* event) override;
	
	void draw(QPainter* painter, MapWidget* widget) override;
	
protected slots:
	void updateDirtyRect();
	
protected:
	void finishDrawing() override;
	void abortDrawing() override;
	
	/**
	 * Deletes the last drawn point.
	 * 
	 * Calls updateRectangle() which will set the than-last point
	 * to constrained_pos_map.
	 */
	void undoLastPoint();
	
	/** Picks a direction from an existing object. */
	void pickDirection(const MapCoordF& coord, MapWidget* widget);
	
	/** Checks if the current drawing direction is parallel to the angle. */
	bool drawingParallelTo(double angle) const;
	
	/** 
	 * Updates the preview after cursor position changes.
	 * May call updateRectangle() internally.
	 */
	void updateHover(bool mouse_down);
	
	/**
	 * Calculates the closing vector.
	 * 
	 * The "closing vector" gives the direction from the current drawing position
	 * perpendicular to the start point.
	 */
	MapCoordF calculateClosingVector() const;
	
	/**
	 * Deletes all points from the preview path which were introduced to close
	 * it temporarily (for preview visualization).
	 */
	void deleteClosePoint();
	
	/**
	 * Recalculates the rectangle shape based on the current input.
	 * 
	 * This will set the "last" point to constrained_pos_map.
	 */
	void updateRectangle();
	
	void updateStatusText();
	
	QPoint click_pos;
	MapCoordF click_pos_map;
	QPoint cur_pos;
	MapCoordF cur_pos_map;
	MapCoordF constrained_pos_map;
	bool dragging         = false;
	bool draw_dash_points = true;
	bool shift_pressed    = false;
	bool ctrl_pressed     = false;
	bool picked_direction = false;
	bool snapped_to_line  = false;
	MapCoord snapped_to_line_a;
	MapCoord snapped_to_line_b;
	
	/**
	 * This can be set to true when a mouse button is pressed down to disable all
	 * actions for the next mouse button release.
	 */
	bool no_more_effect_on_click = false;
	
	/**
	 * List of angles for first, second, etc. edge.
	 * Includes the currently edited angle.
	 * The index of currently edited point in preview_path is angles.size().
	 */
	std::vector< double > angles;
	
	/** Vector in forward drawing direction */
	MapCoordF forward_vector;
	
	QScopedPointer<ConstrainAngleToolHelper> angle_helper;
	QScopedPointer<SnappingToolHelper> snap_helper;
	MapWidget* cur_map_widget;
	
	QPointer<KeyButtonBar> key_button_bar;
	QPointer<QToolButton> dash_points_button;
};


}  // namespace LibreMapper
#endif
