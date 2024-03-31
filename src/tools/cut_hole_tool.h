/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2018-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TOOL_CUT_HOLE_H
#define LIBREMAPPER_TOOL_CUT_HOLE_H

#include <QObject>
#include <QRectF>
#include <QString>

#include "tool.h"

class QAction;
class QCursor;
class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QRectF;

namespace OpenOrienteering {

class DrawLineAndAreaTool;
class MapCoordF;
class MapEditorController;
class MapWidget;
class PathObject;


/** Tool to cut holes into area objects */
class CutHoleTool : public MapEditorTool
{
Q_OBJECT
public:
	/** Enum of different hole types. The CutHoleTool can be used with each of
	 *  the corresponding drawing tools. */
	enum HoleType
	{
		Path = 0,
		Circle = 1,
		Rect = 2
	};
	
	CutHoleTool(MapEditorController* editor, QAction* tool_action, CutHoleTool::HoleType hole_type);
	~CutHoleTool() override;
	
	void init() override;
	const QCursor& getCursor() const override;
	void finishEditing() override;
	
	bool mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseDoubleClickEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	void leaveEvent(QEvent* event) override;
	
	bool keyPressEvent(QKeyEvent* event) override;
	bool keyReleaseEvent(QKeyEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;
	
	void draw(QPainter* painter, MapWidget* widget) override;
	
public slots:
	void objectSelectionChanged();
	void pathDirtyRectChanged(const QRectF& rect);
	void pathAborted();
	void pathFinished(OpenOrienteering::PathObject* hole_path);
	
protected:
	void cutHole(PathObject* edited_path, PathObject* hole_path);
	
	void updateStatusText();
	void updateDirtyRect(const QRectF* path_rect = nullptr);
	
private:
	DrawLineAndAreaTool* path_tool = nullptr;
	CutHoleTool::HoleType hole_type;
};


}  // namespace OpenOrienteering
#endif
