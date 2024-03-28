/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_TEMPLATE_TOOL_MOVE_H
#define OPENORIENTEERING_TEMPLATE_TOOL_MOVE_H

#include <QtGlobal>
#include <QObject>
#include <QString>

#include "core/map_coord.h"
#include "tools/tool.h"

class QAction;
class QCursor;
class QMouseEvent;

namespace OpenOrienteering {

class MapEditorController;
class MapWidget;
class Template;


/** Tool to move a template by hand. */
class TemplateMoveTool : public MapEditorTool
{
Q_OBJECT
public:
	TemplateMoveTool(Template* templ, MapEditorController* editor, QAction* tool_action = nullptr);
	~TemplateMoveTool();
	
	void init() override;
	const QCursor& getCursor() const override;
	
	bool mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	bool mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
public slots:
	void templateDeleted(int index, const OpenOrienteering::Template* temp);
	
private:
	void updateDragging(const MapCoordF& mouse_pos_map);
	
	Template* templ;
	bool dragging = false;
	MapCoordF click_pos_map;
	
	Q_DISABLE_COPY(TemplateMoveTool)
};


}  // namespace OpenOrienteering

#endif
