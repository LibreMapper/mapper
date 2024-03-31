/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_DRAW_POINT_H
#define LIBREMAPPER_DRAW_POINT_H

#include <memory>

#include <QObject>
#include <QPointF>
#include <QString>

#include "tool_base.h"

class QAction;
class QEvent;
class QKeyEvent;
class QPainter;
class QPointF;
class QRectF;

namespace OpenOrienteering {

class MapCoordF;
class MapEditorController;
class MapRenderables;
class MapWidget;
class PointObject;
class Symbol;


/**
 * Tool to draw PointObjects.
 */
class DrawPointTool : public MapEditorToolBase
{
Q_OBJECT
public:
	DrawPointTool(MapEditorController* editor, QAction* tool_action);
	~DrawPointTool() override;
	
protected:
	void initImpl() override;
	
	/**
	 * Tells the tool that the editor's active symbol changed.
	 */
	void activeSymbolChanged(const Symbol* symbol);
	
	/**
	 * Tells the tool that a symbol was deleted from the map.
	 */
	void symbolDeleted(int pos, const Symbol* symbol);
	
	void updatePreviewObject(const MapCoordF& pos);
	
	void createObject();
	
	/**
	 * Calculates the object's rotation for the given mouse position.
	 * 
	 * Return 0 if the user didn't dragged the mouse away the minimum distance
	 * from the click point.
	 */
	double calculateRotation(const QPointF& mouse_pos, const MapCoordF& mouse_pos_map) const;

	void leaveEvent(QEvent* event) override;
	void mouseMove() override;
	void clickPress() override;
	void clickRelease() override;
	void dragStart() override;
	void dragMove() override;
	void dragFinish() override;
	bool keyPress(QKeyEvent* event) override;
	bool keyRelease(QKeyEvent* event) override;
	
	void drawImpl(QPainter* painter, MapWidget* widget) override;
	int updateDirtyRectImpl(QRectF& rect) override;
	void updateStatusText() override;
	void objectSelectionChangedImpl() override;
	
	std::unique_ptr<PointObject> preview_object;
	std::unique_ptr<MapRenderables> renderables;
};


}  // namespace OpenOrienteering

#endif
