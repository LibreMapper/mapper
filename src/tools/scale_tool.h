/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014-2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_SCALE_TOOL_H
#define LIBREMAPPER_SCALE_TOOL_H

#include <QObject>
#include <QString>

#include "core/map_coord.h"
#include "tools/tool_base.h"

class QAction;
class QKeyEvent;
class QPainter;
class QRectF;

namespace LibreMapper {

class MapEditorController;
class MapWidget;


/**
 * Tool to scale objects.
 */
class ScaleTool : public MapEditorToolBase
{
Q_OBJECT
public:
	ScaleTool(MapEditorController* editor, QAction* tool_action);
	~ScaleTool() override;
	
protected:
	void initImpl() override;
	
	void updateStatusText() override;
	
	void clickRelease() override;
	void dragStart() override;
	void dragMove() override;
	void dragFinish() override;
	
	bool keyPressEvent(QKeyEvent* event) override;
	bool keyReleaseEvent(QKeyEvent* event) override;

	void drawImpl(QPainter* painter, MapWidget* widget) override;
	
	int updateDirtyRectImpl(QRectF& rect) override;
	void objectSelectionChangedImpl() override;
	
	MapCoordF scaling_center;
	double reference_length = 0;
	double scaling_factor   = 1;
	bool using_scaling_center = true;
};


}  // namespace LibreMapper

#endif
