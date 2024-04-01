/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_RENDERABLE_IMPLENTATION_H
#define OPENORIENTEERING_RENDERABLE_IMPLENTATION_H

#include <Qt>
#include <QtGlobal>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>

#include "renderable.h"

class QPainter;
class QPointF;

namespace OpenOrienteering {

class AreaSymbol;
class LineSymbol;
class MapColor;
class MapCoordF;
class PathPartVector;
class PointSymbol;
class TextObject;
class TextSymbol;
class VirtualPath;


/** Renderable for displaying a filled dot. */
class DotRenderable : public Renderable
{
public:
	DotRenderable(const PointSymbol* symbol, MapCoordF coord);
	void render(QPainter& painter, const RenderConfig& config) const override;
	PainterConfig getPainterConfig(const QPainterPath* clip_path = nullptr) const override;
};

/** Renderable for displaying a circle. */
class CircleRenderable : public Renderable
{
public:
	CircleRenderable(const PointSymbol* symbol, MapCoordF coord);
	void render(QPainter& painter, const RenderConfig& config) const override;
	PainterConfig getPainterConfig(const QPainterPath* clip_path = nullptr) const override;
	
protected:
	const qreal line_width;
	QRectF rect;
};

/** Renderable for displaying a line. */
class LineRenderable : public Renderable
{
public:
	LineRenderable(const LineSymbol* symbol, const VirtualPath& virtual_path, bool closed);
	LineRenderable(const LineSymbol* symbol, QPointF first, QPointF second);
	void render(QPainter& painter, const RenderConfig& config) const override;
	PainterConfig getPainterConfig(const QPainterPath* clip_path = nullptr) const override;
	
protected:
	void extentIncludeCap(quint32 i, qreal half_line_width, bool end_cap, const LineSymbol* symbol, const VirtualPath& path);
	
	void extentIncludeJoin(quint32 i, qreal half_line_width, const LineSymbol* symbol, const VirtualPath& path);
	
	const qreal line_width;
	QPainterPath path;
	Qt::PenCapStyle cap_style;
	Qt::PenJoinStyle join_style;
};

/** Renderable for displaying an area. */
class AreaRenderable : public Renderable
{
public:
	AreaRenderable(const AreaSymbol* symbol, const PathPartVector& path_parts);
	AreaRenderable(const AreaSymbol* symbol, const VirtualPath& path);
	void render(QPainter& painter, const RenderConfig& config) const override;
	PainterConfig getPainterConfig(const QPainterPath* clip_path = nullptr) const override;
	
	inline const QPainterPath* painterPath() const;
	
protected:
	void addSubpath(const VirtualPath& virtual_path);
	
	QPainterPath path;
};

/** Renderable for displaying text. */
class TextRenderable : public Renderable
{
public:
	TextRenderable(const TextSymbol* symbol, const TextObject* text_object, const MapColor* color, double anchor_x, double anchor_y);
	PainterConfig getPainterConfig(const QPainterPath* clip_path = nullptr) const override;
	void render(QPainter& painter, const RenderConfig& config) const override;
	
protected:
	void renderCommon(QPainter& painter, const RenderConfig& config) const;
	
	QPainterPath path;
	qreal anchor_x;
	qreal anchor_y;
	qreal rotation;
	qreal scale_factor;
};

/** Renderable for displaying framing line for text. */
class TextFramingRenderable : public TextRenderable
{
public:
	TextFramingRenderable(const TextSymbol* symbol, const TextObject* text_object, const MapColor* color, double anchor_x, double anchor_y);
	PainterConfig getPainterConfig(const QPainterPath* clip_path = nullptr) const override;
	void render(QPainter& painter, const RenderConfig& config) const override;
	
protected:
	qreal framing_line_width;
};



// ### AreaRenderable inline code ###

const QPainterPath* AreaRenderable::painterPath() const
{
	return &path;
}


}  // namespace OpenOrienteering

#endif
