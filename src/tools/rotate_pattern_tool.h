/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2014, 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_TOOL_ROTATE_PATTERN_H
#define OPENORIENTEERING_TOOL_ROTATE_PATTERN_H

#include <QObject>
#include <QString>

#include "tools/tool_base.h"

class QAction;
class QKeyEvent;
class QPainter;
class QRectF;

namespace OpenOrienteering {

class MapEditorController;
class MapWidget;


/**
 * Tool to rotate patterns of area objects, or to set the direction of rotatable point objects.
 */
class RotatePatternTool : public MapEditorToolBase
{
Q_OBJECT
public:
	RotatePatternTool(MapEditorController* editor, QAction* tool_action);
	~RotatePatternTool() override;
	
	bool keyPressEvent(QKeyEvent* event) override;
	bool keyReleaseEvent(QKeyEvent* event) override;
	
protected:
	void clickPress() override;
	void clickRelease() override;
	void dragStart() override;
	void dragMove() override;
	void dragFinish() override;
	
	void initImpl() override;
	void drawImpl(QPainter* painter, MapWidget* widget) override;
	int updateDirtyRectImpl(QRectF& rect) override;
	void updateStatusText() override;
	void objectSelectionChangedImpl() override;
};


}  // namespace OpenOrienteering

#endif
