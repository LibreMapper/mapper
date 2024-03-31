/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TOOL_PAN_H
#define LIBREMAPPER_TOOL_PAN_H

#include <QObject>
#include <QString>

#include "tools/tool_base.h"

class QAction;

namespace OpenOrienteering {

class MapEditorController;


/** 
 * Tool to pan the map.
 */
class PanTool : public MapEditorToolBase
{
Q_OBJECT
public:
	PanTool(MapEditorController* editor, QAction* tool_action);
	~PanTool() override;
	
protected:
	void updateStatusText() override;
	void objectSelectionChangedImpl() override;
	void clickPress() override;
	void dragStart() override;
	void dragMove() override;
	void dragFinish() override;
	void dragCanceled() override;
};


}  // namespace OpenOrienteering

#endif
