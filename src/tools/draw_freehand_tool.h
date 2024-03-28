/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2014, 2015, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_DRAW_FREEHAND_TOOL_H
#define OPENORIENTEERING_DRAW_FREEHAND_TOOL_H

#include <cstddef>
#include <vector>

#include <QtGlobal>
#include <QObject>
#include <QPoint>
#include <QString>

#include "core/map_coord.h"
#include "tools/draw_line_and_area_tool.h"

class QAction;
class QCursor;
class QKeyEvent;
class QMouseEvent;
class QPainter;

namespace OpenOrienteering {

class MapEditorController;
class MapWidget;


/** Tool for free-hand drawing. */
class DrawFreehandTool : public DrawLineAndAreaTool
{
Q_OBJECT
public:
	DrawFreehandTool(MapEditorController* editor, QAction* tool_action, bool is_helper_tool);
	~DrawFreehandTool() override;
	
	void init() override;
	const QCursor& getCursor() const override;
	
	bool mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
	bool keyPressEvent(QKeyEvent* event) override;
	
	void draw(QPainter* painter, MapWidget* widget) override;
	
protected:
	void finishDrawing() override;
	void abortDrawing() override;
	
	void updatePath();
	void setDirtyRect();
	void updateStatusText();
	
private:
	void checkLineSegment(std::size_t first, std::size_t last);
	
	std::vector<bool> point_mask;
	qreal split_distance_sq;
	
	QPoint last_pos;
	QPoint cur_pos;
	MapCoordF cur_pos_map;
};


}  // namespace OpenOrienteering

#endif
