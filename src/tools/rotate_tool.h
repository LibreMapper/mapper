/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2014, 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TOOL_ROTATE_H
#define LIBREMAPPER_TOOL_ROTATE_H

#include <QtGlobal>
#include <QObject>
#include <QString>

#include "core/map_coord.h"
#include "tools/tool_base.h"

class QAction;
class QKeyEvent;
class QPainter;
class QRectF;

namespace OpenOrienteering {

class MapEditorController;
class MapWidget;


/**
 * Tool to rotate objects.
 */
class RotateTool : public MapEditorToolBase
{
	Q_OBJECT
	
public:
	RotateTool(MapEditorController* editor, QAction* tool_action);
	~RotateTool() override;
	
protected:
	void initImpl() override;
	
	void mouseMove() override;
	void clickRelease() override;
	void dragStart() override;
	void dragMove() override;
	void dragFinish() override;
	bool keyPress(QKeyEvent* event) override;
	bool keyRelease(QKeyEvent* event) override;
	
	void drawImpl(QPainter* painter, MapWidget* widget) override;
	
	int updateDirtyRectImpl(QRectF& rect) override;
	void objectSelectionChangedImpl() override;
	
	void updateStatusText() override;
	
	// Mouse handling
	MapCoordF rotation_center;
	qreal original_rotation = 0;
	qreal current_rotation  = 0;
};


}  // namespace OpenOrienteering
#endif
