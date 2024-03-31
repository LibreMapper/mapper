/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_FILL_TOOL_H
#define LIBREMAPPER_FILL_TOOL_H

#include <vector>

#include <QImage>
#include <QObject>
// IWYU pragma: no_include <QRectF>
#include <QString>
#include <QTransform>

#include "tool_base.h"

class QAction;
class QPainter;
class QPoint;
class QRectF;

namespace LibreMapper {

class Map;
class MapEditorController;
class RenderConfig;
class Symbol;


/** 
 * Tool to fill bounded areas with PathObjects.
 */
class FillTool : public MapEditorToolBase
{
Q_OBJECT
public:
	FillTool(MapEditorController* editor, QAction* tool_action);
	~FillTool() override;
	
protected slots:
	void setDrawingSymbol(const LibreMapper::Symbol* symbol);
	
protected:
	void updateStatusText() override;
	void objectSelectionChangedImpl() override;
	
	void clickPress() override;
	
	/**
	 * Tries to apply the fill tool at the current click position,
	 * rasterizing the given extent of the map.
	 * Returns -1 for abort, 0 for unsuccessful, 1 for successful.
	 */
	int fill(const QRectF& extent);
	
	/**
	 * Rasterizes an area of the current map part with the given extent into an image.
	 * 
	 * The pixels encodes object IDs (with alpha = 255). The background is white.
	 * Returns the image and the used map-to-image transform.
	 */
	QImage rasterizeMap(const QRectF& extent, QTransform& out_transform);
	
	/**
	 * Helper method for rasterizeMap().
	 */
	void drawObjectIDs(Map* map, QPainter* painter, const RenderConfig& config);
	
	/**
	 * Constructs the boundary around an area of free pixels in the given image.
	 * 
	 * The discovered boundary is stored as a vector of pixel positions.
	 * The tracing starts from the free_pixel/boundary_pixel pair, where
	 * free_pixel references a free (transparent) pixel next to boundary pixel
	 * which reference an obstructed pixel of area to be traced.
	 * 
	 * Returns:
	 * -1 if running out of the image borders,
	 *  0 if the tracing fails because the start is not included in the shape,
	 *  1 if the tracing succeeds.
	 */
	int traceBoundary(const QImage& image, const QPoint& free_pixel, const QPoint& boundary_pixel, std::vector<QPoint>& out_boundary);
	
	/**
	 * Creates a fill object for the given image, boundary vector (of pixel positions) and transform.
	 * Returns false if the creation fails.
	 */
	bool fillBoundary(const QImage& image, const std::vector<QPoint>& boundary, const QTransform& image_to_map);
	
	const Symbol* drawing_symbol;
};


}  // namespace LibreMapper

#endif
