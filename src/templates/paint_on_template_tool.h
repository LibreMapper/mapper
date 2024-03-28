/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2017-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_TEMPLATE_TOOL_PAINT_H
#define OPENORIENTEERING_TEMPLATE_TOOL_PAINT_H

#include <vector>

#include <Qt>
#include <QtGlobal>
#include <QColor>
#include <QFlags>
#include <QObject>
#include <QPointer>
#include <QRectF>
#include <QString>

#include "core/map_coord.h"
#include "templates/template.h"
#include "tools/tool.h"

class QAction;
class QActionGroup;
class QCursor;
class QMouseEvent;
class QPainter;

namespace OpenOrienteering {

class ActionGridBar;
class MapEditorController;
class MapWidget;


/**
 * A tool to paint on image templates.
 */
class PaintOnTemplateTool : public MapEditorTool
{
Q_OBJECT
public:
	enum ErasingOption {
		NoErasing = 0,
		ExplicitErasing = 1,
		RightMouseButtonErasing = 2
	};
	Q_DECLARE_FLAGS(ErasingOptions, ErasingOption)
	
	PaintOnTemplateTool(MapEditorController* editor, QAction* tool_action);
	~PaintOnTemplateTool() override;
	
	void setTemplate(Template* temp);
	
	void init() override;
	const QCursor& getCursor() const override;
	
	bool mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
	void draw(QPainter* painter, MapWidget* widget) override;
	
protected:
	void templateAboutToBeDeleted(int pos, Template* temp);
	
public:
	Template::ScribbleOptions options() const noexcept { return paint_options; }
	void setOptions(Template::ScribbleOptions options);
	
	const QColor& color() const noexcept { return paint_color; }
	void setColor(const QColor& color);
	
	void undoSelected();
	void redoSelected();

protected:
	ActionGridBar* makeToolBar();
	
private:
	ErasingOptions erasing = {};
	bool dragging = false;
	Template::ScribbleOptions paint_options = {};
	QColor paint_color = Qt::black;
	QRectF map_bbox;
	std::vector<MapCoordF> coords;
	
	Template* temp = nullptr;
	QPointer<ActionGridBar> widget;
	QAction* background_drawing_action = nullptr;
	QActionGroup* fill_options = nullptr;
	
	static int erase_width;
	
	Q_DISABLE_COPY(PaintOnTemplateTool)
};


}  // namespace OpenOrienteering


Q_DECLARE_OPERATORS_FOR_FLAGS(OpenOrienteering::PaintOnTemplateTool::ErasingOptions)


#endif
