/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_DRAW_LINE_AND_AREA_H
#define LIBREMAPPER_DRAW_LINE_AND_AREA_H

#include <memory>
#include <vector>

#include <QObject>
#include <QRectF>
#include <QString>

#include "tools/tool.h"

class QAction;
class QEvent;
class QPainter;
class QRectF;

namespace LibreMapper {

class CombinedSymbol;
class LineSymbol;
struct LineSymbolBorder;
class MapCoordF;
class MapEditorController;
class MapRenderables;
class MapWidget;
class PathObject;
class PointObject;
class PointSymbol;
class Symbol;


/**
 * Base class for drawing tools for line and area symbols.
 * Provides some common functionality, e.g. displaying the preview objects or
 * coping with changing symbols.
 */
class DrawLineAndAreaTool : public MapEditorTool
{
Q_OBJECT
public:
	DrawLineAndAreaTool(MapEditorController* editor, Type type, QAction* tool_action, bool is_helper_tool);
	~DrawLineAndAreaTool() override;
	
	void leaveEvent(QEvent* event) override;
	void finishEditing() override;
	
signals:
	void dirtyRectChanged(const QRectF& rect);
	void pathAborted();
	void pathFinished(LibreMapper::PathObject* path);
	
protected slots:
	virtual void setDrawingSymbol(const LibreMapper::Symbol* symbol);
	
protected:
	/**
	 * Creates point objects and symbols which serve as a preview for
	 * drawing the selected symbol.
	 */
	void createPreviewPoints();
	
	/** Creates preview point symbols for the given symbol. */
	void addPreviewPointSymbols(const Symbol* symbol);
	
	/** Creates preview point symbols for the border of the given line symbol. */
	void addPreviewPointSymbolsForBorder(const LineSymbol* line, const LineSymbolBorder* border);
	
	/**
	 * Sets the position of the preview objects (after cursor movements).
	 * @param map_coord The new position.
	 * @param points_index The index of the points set; there are two sets,
	 *     so the preview points can be displayed at two positions at the same time.
	 */
	void setPreviewPointsPosition(const MapCoordF& map_coord, int points_index = 0);
	
	/** Hides all preview points. */
	void hidePreviewPoints();
	
	
	/** Does necessary preparations to start drawing. */
	void startDrawing();
	
	/** Calls update() on the preview path, correctly handling its renderables. */
	virtual void updatePreviewPath();
	
	/** Aborts drawing. */
	virtual void abortDrawing();
	
	/** Finishes drawing, creating a new undo step. */
	virtual void finishDrawing();
	
	/** Finishes drawing, appending to the given object if not nullptr. */
	void finishDrawing(PathObject* append_to_object);
	
	
	/** Deletes preview all objects and points. */
	void deletePreviewObjects();
	
	/** Extends the rect to cover all preview objects. */
	void includePreviewRects(QRectF& rect);
	
	/** Draws the preview objects. */
	void drawPreviewObjects(QPainter* painter, MapWidget* widget);
	
	
	std::vector<PointSymbol*> preview_point_symbols;
	std::vector<bool> preview_point_symbols_external;
	std::vector<PointObject*> preview_points[2];
	
	std::unique_ptr<CombinedSymbol> path_combination;
	
	std::unique_ptr<MapRenderables> renderables;
	
	const Symbol* drawing_symbol = nullptr;
	PathObject* preview_path     = nullptr;
	int preview_point_radius     = 0;
	bool preview_points_shown    = false;
	bool is_helper_tool          = false;
	
};


}  // namespace LibreMapper
#endif
