/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2016  Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_PRINT_TOOL_H
#define LIBREMAPPER_PRINT_TOOL_H

#ifdef QT_PRINTSUPPORT_LIB

#include <QObject>
#include <QPoint>

#include "core/map_coord.h"
#include "tools/tool.h"

class QCursor;
class QMouseEvent;
class QPainter;

namespace LibreMapper {

class MapEditorController;
class MapPrinter;
class MapWidget;


/**
 * The PrintTool lets the user see and modify the print area on the map
 * by dragging
 * 
 * It interacts with a MapEditorController and a PrintWidget which are set in
 * the constructor.
 */
class PrintTool : public MapEditorTool
{
Q_OBJECT
public:
	/** Constructs a new PrintTool to configure the given map printer in the 
	 *  context of the editor.
	 * 
	 *  The parameters must not be null. */
	PrintTool(MapEditorController* editor, MapPrinter* map_printer);
	
	~PrintTool() override;
	
	/** Notifies the tool that it becomes active. */
	void init() override;
	
	/** Always returns the tool's default cursor. */
	const QCursor& getCursor() const override;
	
	/** Starts a dragging interaction. */
	bool mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
	/** Updates the state of a running dragging interaction. When not dragging,
	 *  it will update the cursor to indicate a possible interaction. */
	bool mouseMoveEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
	/** Finishes dragging interactions. */
	bool mouseReleaseEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* widget) override;
	
	/** Draws a visualization of the print area the map widget. */
	void draw(QPainter* painter, MapWidget* widget) override;
	
public slots:
	/** Updates the print area visualization in the map editors. */
	void updatePrintArea();
	
protected:
	/** Modifies the print area while dragging.
	 *  This must not be called when the region is Outside. */
	void updateDragging(const MapCoordF& mouse_pos_map);
	
	/** Updates the current interaction region.
	 *  This must not be called during dragging. */
	void mouseMoved(const MapCoordF& mouse_pos_map, MapWidget* widget);
	
	/** Regions of interaction with the print area. */
	enum InteractionRegion {
		Inside            = 0x00,
		Outside           = 0x01,
		LeftBorder        = 0x02,
		TopLeftCorner     = 0x06,
		TopBorder         = 0x04,
		TopRightCorner    = 0x0c,
		RightBorder       = 0x08,
		BottomRightCorner = 0x18,
		BottomBorder      = 0x10,
		BottomLeftCorner  = 0x12,
		Unknown           = 0xFF
	};
	
	/** The map printer this tool is operation on. */
	MapPrinter* const map_printer;
	
	/** The region of the print area where the current interaction takes place. */
	InteractionRegion region;
	
	/** Indicates whether an interaction is taking place at the moment. */
	bool dragging;
	
	/** The screen position where the initial click was made. */
	QPoint click_pos;
	
	/** The map position where the initial click was made. */
	MapCoordF click_pos_map;
};


}  // namespace LibreMapper

#endif // QT_PRINTSUPPORT_LIB

#endif // LIBREMAPPER_PRINT_TOOL_H
