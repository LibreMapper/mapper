/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_DRAW_PATH_H
#define LIBREMAPPER_DRAW_PATH_H

#include <memory>

#include <QtGlobal>
#include <QObject>
#include <QPoint>
#include <QPointer>
#include <QString>

#include "core/map_coord.h"
#include "tools/draw_line_and_area_tool.h"

class QAction;
class QCursor;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QToolButton;

namespace LibreMapper {

class AzimuthInfoHelper;
class ConstrainAngleToolHelper;
class FollowPathToolHelper;
class KeyButtonBar;
class MapEditorController;
class MapWidget;
class PathObject;
class SnappingToolHelper;
class SnappingToolHelperSnapInfo;
class Symbol;


/** 
 * Tool to draw arbitrarily shaped PathObjects.
 */
class DrawPathTool : public DrawLineAndAreaTool
{
Q_OBJECT
public:
	DrawPathTool(MapEditorController* editor, QAction* tool_action, bool is_helper_tool, bool allow_closing_paths);
	~DrawPathTool() override;
	
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
	void setDrawingSymbol(const LibreMapper::Symbol* symbol) override;
	
	/** This slot listens to changes in the map's object selection. */
	virtual void objectSelectionChanged();
	
protected:
	void updatePreviewPath() override;
	/** Should be called when moving the cursor without the draw button being held */
	void updateHover();
	/** Called by updateHover() if the user is currently drawing */
	void updateDrawHover();
	/** Updates the last three points of the path to form a bezier curve */
	void createPreviewCurve(MapCoord position, qreal direction);
	/** Closes the preview path */
	void closeDrawing();
	void finishDrawing() override;
	void abortDrawing() override;
	void undoLastPoint();
	/** When not drawing but when the current selection is a single path,
	 *  removes the last point from that path, or deletes the whole path if
	 *  there are too few remaining points.
	 *  Returns true if the path was modified or deleted. */
	bool removeLastPointFromSelectedPath();
	void updateAngleHelper();
	bool pickAngle(const MapCoordF& coord, MapWidget* widget);
	void updateSnapHelper();
	
	/** Starts appending to another, existing object */
	void startAppending(SnappingToolHelperSnapInfo& snap_info);
	
	/** Starts following another, existing path */
	void startFollowing(SnappingToolHelperSnapInfo& snap_info, const MapCoord& snap_coord);
	void updateFollowing();
	void finishFollowing();
	
	/**
	 * Checks if the user dragged the mouse away a certain minimum distance from
	 * the click point and if yes, returns the drag angle, otherwise returns 0.
	 */
	qreal calculateRotation(const QPoint& mouse_pos, const MapCoordF& mouse_pos_map) const;
	/**
	 * Activates or deactivates dash point drawing depending on if a line symbol
	 * with dash symbols is selected.
	 */
	void updateDashPointDrawing();
	void updateStatusText();
	
	
	QPointer<KeyButtonBar> key_button_bar;
	QPointer<QToolButton> dash_points_button;
	QPointer<QToolButton> azimuth_button;
	
	MapWidget* cur_map_widget;
	
	QPoint click_pos;
	MapCoordF click_pos_map;
	
	QPoint cur_pos;
	MapCoordF cur_pos_map;
	
	/** The beginning of the current curve in the preview path. */
	MapCoordF previous_pos_map;  
	/** A control point defining the tangent at the beginning of the current curve. */
	MapCoordF previous_drag_map;
	
	std::unique_ptr<ConstrainAngleToolHelper> angle_helper;
	MapCoordF constrained_pos_map;
	
	std::unique_ptr<AzimuthInfoHelper> azimuth_helper;

	std::unique_ptr<SnappingToolHelper> snap_helper;
	
	PathObject* append_to_object;
	std::unique_ptr<PathObject> followed_path;
	std::unique_ptr<LineSymbol> covering_white_dot;
	std::unique_ptr<LineSymbol> covering_red_dot;
	std::unique_ptr<CombinedSymbol> follow_highlight_symbol;
	
	std::unique_ptr<FollowPathToolHelper> follow_helper;
	MapCoordVector::size_type follow_start_index;
	
	qreal previous_point_direction = 0;
	
	bool allow_closing_paths = true;
	bool ctrl_pressed     = false;
	bool shift_pressed    = false;
	bool left_mouse_down  = false;
	bool appending        = false;
	bool following        = false;
	bool picking_angle    = false; ///< Indicates picking of the initial angle from an object.
	bool picked_angle     = false; ///< Indicates an active angle picked from another object.
	bool dragging         = false;
	bool draw_dash_points = false;
	bool create_segment   = false;
	bool create_spline_corner = false; ///< For drawing bezier splines without parallel handles
	bool path_has_preview_point = false;
	bool previous_point_is_curve_point = false;
	bool created_point_at_last_mouse_press = false;  ///< Used for finishing on double click.
	bool finished_path_is_selected = false;  ///< True just after finishing a path
	
};


}  // namespace LibreMapper

#endif
