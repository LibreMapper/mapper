/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2023 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TOOL_HELPERS_H
#define LIBREMAPPER_TOOL_HELPERS_H

#include <cstddef>
#include <memory>
#include <set>

#include <QtGlobal>
#include <QColor>
#include <QFont>
#include <QObject>
#include <QRectF>
#include <QString>

#include "core/map_coord.h"
#include "core/path_coord.h"
#include "tools/point_handles.h"

class QPainter;
class QPoint;
class QPointF;
class QRectF;
class QWidget;

namespace LibreMapper {

class Map;
class MapEditorTool;
class MapWidget;
class Object;
class PathObject;
class Symbol;


/**
 * Helper class to constrain cursor positions to specific lines / directions
 * which originate from a given point
 */
class ConstrainAngleToolHelper : public QObject
{
Q_OBJECT
public:
	/**
	 * Constructs a helper without allowed angles.
	 * Use addAngle() and / or addAngles() to add allowed lines.
	 */
	ConstrainAngleToolHelper();
	
	~ConstrainAngleToolHelper() override;
	
	
	/** Sets the center of the lines */
	void setCenter(const MapCoordF& center);
	
	/**
	 * Adds a single allowed angle. Zero is to the right,
	 * the direction counter-clockwise in Qt's coordinate system.
	 */
	void addAngle(qreal angle);
	
	/**
	 * Adds a circular set of allowed angles, starting from 'base' with
	 * interval 'stepping'. Zero is to the right, the direction counter-clockwise
	 * in Qt's coordinate system.
	 */
	void addAngles(qreal base, qreal stepping);
	
	/**
	 * Like addAngles, but in degrees. Helps to avoid floating-point
	 * inaccuracies if using angle steppings like 15 degrees which could lead
	 * to two near-zero allowed angles otherwise.
	 */
	void addAnglesDeg(qreal base, qreal stepping);
	
	/**
	 * Adds the default angles given by the MapEditor_FixedAngleStepping setting.
	 * Usage of this method has the advantage that the stepping is updated
	 * automatically when the setting is changed.
	 */
	void addDefaultAnglesDeg(qreal base);
	
	/** Removes all allowed angles */
	void clearAngles();
	
	/**
	 * Get the cursor position, projected onto the closest of the allowed angles.
	 * Returns the chosen angle and marks it as active.
	 */
	double getConstrainedCursorPos(const QPoint& in_pos, QPointF& out_pos, MapWidget* widget);
	
	/**
	 * Get the cursor position, projected onto the closest of the allowed angles.
	 * Returns the chosen angle and marks it as active.
	 */
	double getConstrainedCursorPosMap(const MapCoordF& in_pos, MapCoordF& out_pos);
	
	/** Combination of the above methods for convenience */
	double getConstrainedCursorPositions(const MapCoordF& in_pos_map,
		MapCoordF& out_pos_map, QPointF& out_pos, MapWidget* widget);
	
	/**
	 * Activates or deactivates this tool.
	 * 
	 * If deactivated, it does nothing. This is just for convenience to avoid
	 * if-clauses in places where this tool is used sometimes.
	 * If activated, the center is replaced with the new one.
	 * 
	 * TODO: This is ugly and should be two functions. Do not use in new code.
	 *       Instead of setting the center when activating, the center should
	 *       be always kept up-to-date independently of the activation which
	 *       is more intuitive.
	 */
	void setActive(bool active, const MapCoordF& center);
	
	/** Version of setActive() which does not override the center */
	void setActive(bool active);
	
	bool isActive() const { return active; }
	
	/**
	 * Draws the set of allowed angles as lines radiating out from the
	 * center point. The active angle, if any, is highlighted.
	 */
	void draw(QPainter* painter, MapWidget* widget);
	
	/** Includes this helper's drawing region in the given rect. */
	void includeDirtyRect(QRectF& rect);
	
	/** Returns the radius of the visualization in pixels */
	int getDisplayRadius() const { return active ? 40 : 0; }
	
	
	void settingsChanged();
	
	
signals:
	/** Emitted when the angle the cursor position is constrained to changes */
	void activeAngleChanged() const;  // clazy:exclude=const-signal-or-slot
	
	/**
	 * Emitted whenever the display of this tool helper changes.
	 * This is when the active angle changes or the tool is activated / deactivated.
	 */
	void displayChanged() const;  // clazy:exclude=const-signal-or-slot
	
	
private:
	inline void emitActiveAngleChanged() const {emit activeAngleChanged(); emit displayChanged();}
	
	/** The active angle or a negative number if no angle is active */
	qreal active_angle = -1;
	/** The set of allowed angles. Values are in the range [0, 2*M_PI) */
	std::set<qreal> angles;
	/** The center point of all lines */
	MapCoordF center = {};
	/** Is this helper active? */
	bool active = true;
	
	bool have_default_angles_only = false;
	qreal default_angles_base = 1;
};



struct SnappingToolHelperSnapInfo;

/**
 * Helper class to snap to existing objects or a grid on the map.
 */
class SnappingToolHelper : public QObject
{
Q_OBJECT
public:
	enum SnapObjects
	{
		NoSnapping = 0,
		ObjectCorners = 1 << 0,
		ObjectPaths = 1 << 1,
		GridCorners = 1 << 2,
		LineBorders = 1 << 3,
		
		AllTypes = 1 + 2 + 4 + 8
	};
	
	/**
	 * Constructs a snapping tool helper. By default it is disabled
	 * (filter set to NoSnapping).
	 */
	SnappingToolHelper(MapEditorTool* tool, SnapObjects filter = NoSnapping);
	
	~SnappingToolHelper() override;
	
	/** Constrain the objects to snap onto. */
	void setFilter(SnapObjects filter);
	SnapObjects getFilter() const;
	
	/**
	 * Snaps the given position to the closest snapping object, or returns
	 * the original position if no snapping object is close enough.
	 * Internally remembers the position so the next call to draw() will
	 * draw the snap mark there.
	 * 
	 * If the info parameter is set, information about the object
	 * snapped onto is returned there. The snap_distance parameter can be
	 * used to set the maximum snap distance. If it is negative,
	 * the corresponding application setting will be used.
	 * 
	 * TODO: widget parameter is only used for getMapView(). Replace by view parameter?
	 */
	MapCoord snapToObject(const MapCoordF& position, MapWidget* widget, SnappingToolHelperSnapInfo* info = nullptr, Object* exclude_object = nullptr);
	
	/**
	 * Checks for existing objects in map at position and if one is found,
	 * returns true and sets related angles in angle_tool.
	 * Internally remembers the position so the next call to draw() will
	 * draw the snap mark there.
	 */
	bool snapToDirection(const MapCoordF& position, MapWidget* widget, ConstrainAngleToolHelper* angle_tool, MapCoord* out_snap_position = nullptr);
	
	/** Draws the snap mark which was last returned by snapToObject(). */
	void draw(QPainter* painter, MapWidget* widget);
	
	/** Includes this helper's drawing region in the given rect. */
	void includeDirtyRect(QRectF& rect);
	
	/**
	 * Returns the radius of the visualization in pixels.
	 * 
	 * \todo Return a dynamic value, or adjust for screen density.
	 */
	inline int getDisplayRadius() const {return (snapped_type != NoSnapping) ? 12 : 0;}
	
signals:
	/** Emitted whenever the snap mark changes position. */
	void displayChanged() const;  // clazy:exclude=const-signal-or-slot
	
private:
	PointHandles point_handles;
	SnapObjects filter;
	Map* map;
	
	MapCoord snap_mark = {};
	SnapObjects snapped_type = NoSnapping;
	
};



/**
 * Information returned from a snap process from SnappingToolHelper.
 */
struct SnappingToolHelperSnapInfo
{
	/** Type of object snapped onto */
	SnappingToolHelper::SnapObjects type;
	
	/** Object snapped onto, if type is ObjectCorners or ObjectPaths, else nullptr */
	Object* object;
	
	/** Index of the map coordinate which was snapped onto if type is ObjectCorners,
	 *  else -1 (not snapped to a specific coordinate) */
	MapCoordVector::size_type coord_index;
	
	/** The closest point on the snapped path is returned
	 *  in path_coord if type == ObjectPaths  */
	PathCoord path_coord;
	
	/**
	 * If snapping to a border line, this is a reference to the border's temporary path.
	 */
	std::shared_ptr<PathObject> border_path;
	
};


/**
 * Helper class to 'follow' (i.e. extract continuous parts from) PathObjects.
 * 
 * A FollowPathToolHelper can be reused for following different paths.
 */
class FollowPathToolHelper
{
public:
	/**
	 * Returns true iff following can be started from the given snapping information.
	 */
	bool canStartFollowing(const SnappingToolHelperSnapInfo& snap_info) const noexcept;
	
	/**
	 * Starts following from the given snapping information.
	 */
	void startFollowing(const SnappingToolHelperSnapInfo& snap_info);
	
	/**
	 * Updates the process and returns the followed part of the path.
	 * 
	 * Returns a pointer that owns nothing if the following failed, e.g.
	 * because the path coord is on another path part than the beginning or
	 * path and end are the same.
	 */
	std::unique_ptr<PathObject> updateFollowing(const PathCoord& end_coord);
	
	
	/**
	 * Returns the object which is being followed.
	 */
	const PathObject* followedObject() const { return path; }
	
	/**
	 * Returns the index of the path part which is being followed.
	 */
	std::size_t partIndex() const { return part_index; }
	
private:
	std::shared_ptr<PathObject> border_path;
	const PathObject* path = nullptr;
	
	PathCoord::length_type start_clen;
	PathCoord::length_type end_clen;
	std::size_t part_index;
	bool drag_forward;
};



/**
 * A utility for displaying azimuth and distance while drawing.
 */
class AzimuthInfoHelper
{
protected:
	static constexpr int text_offset = 25;
	
	QColor text_color;
	QFont  text_font;
	QString azimuth_template;
	QString distance_template;
	QRectF display_rect;
	int line_0_offset;
	int line_1_offset; 
	bool active = false;

public:
	AzimuthInfoHelper(const QWidget* widget, QColor color);
	
	bool isActive() const { return active; }

	void setActive(bool active);
	
	/** Returns this helper's drawing region. */
	QRectF dirtyRect(MapWidget* widget, const MapCoordF& pos_map) const;
	
	/** Draws the azimuth and distance info text. */
	void draw(QPainter* painter, const MapWidget* widget, const Map* map,
	          const MapCoordF& start_pos, const MapCoordF& end_pos);

};

/**
 * Helper function to determine whether a symbol that is either a line symbol
 * or contains line symbols as part of a combined symbol contains a dash symbol.
 * Function is used by DrawPathTool::updateDashPointDrawing() and 
 * EditPointTool::addDashPointDefault() in relation to setting and changing dash points.
 */

bool symbolContainsDashSymbol(const Symbol* symbol);

}  // namespace LibreMapper

#endif
