/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2025 Andreas Bertheussen (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_BOX_ZOOM_TOOL_H
#define LIBREMAPPER_BOX_ZOOM_TOOL_H

#include <QObject>
#include <QString>

#include "tools/tool_base.h"

class QAction;
class QKeyEvent;
class QPainter;
class QRectF;

namespace LibreMapper {

class MapEditorController;
class MapWidget;


/**
 * Tool to zoom to a user defined region.
 */
class BoxZoomTool : public MapEditorToolBase
{
	Q_OBJECT
public:
	BoxZoomTool(MapEditorController* editor, QAction* tool_action);
	~BoxZoomTool() override;

protected:
	void updateStatusText() override;
	void objectSelectionChangedImpl() override;
	void clickPress() override;
	bool keyPress(QKeyEvent* event) override;
	void dragStart() override;
	void dragMove() override;
	void dragFinish() override;
	void dragCanceled() override;

	void drawImpl(QPainter* painter, MapWidget* widget) override;
	int updateDirtyRectImpl(QRectF& rect) override;
};


}  // namespace LibreMapper

#endif
