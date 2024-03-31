/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Thomas Schöps (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_DRAW_POINT_GPS_H
#define LIBREMAPPER_DRAW_POINT_GPS_H

#include <QObject>
#include <QPointer>
#include <QScopedPointer>
#include <QString>

#include "tools/tool_base.h"

class QAction;
class QKeyEvent;
class QLabel;
class QPainter;
class QRectF;

namespace LibreMapper {

class GPSDisplay;
class MapCoordF;
class MapEditorController;
class MapRenderables;
class MapWidget;
class PointObject;
class Symbol;


/**
 * Tool to draw a PointObject at the GPS position.
 */
class DrawPointGPSTool : public MapEditorToolBase
{
Q_OBJECT
public:
	DrawPointGPSTool(GPSDisplay* gps_display, MapEditorController* editor, QAction* tool_action);
	~DrawPointGPSTool() override;
	
public slots:
	void newGPSPosition(const LibreMapper::MapCoordF& coord, float accuracy);
	
protected slots:
	void activeSymbolChanged(const LibreMapper::Symbol* symbol);
	void symbolDeleted(int pos, const LibreMapper::Symbol* old_symbol);
	
protected:
	void initImpl() override;
	int updateDirtyRectImpl(QRectF& rect) override;
	void drawImpl(QPainter* painter, MapWidget* widget) override;
	void updateStatusText() override;
	void objectSelectionChangedImpl() override;
	
	void clickRelease() override;
	bool keyPress(QKeyEvent* event) override;
	
	double x_sum;
	double y_sum;
	double weights_sum;
	
	const Symbol* last_used_symbol = nullptr;
	PointObject* preview_object = nullptr;
	QScopedPointer<MapRenderables> renderables;
	QPointer<QLabel> help_label;
};


}  // namespace LibreMapper
#endif
