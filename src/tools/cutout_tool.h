/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012, 2013, 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_CUTOUT_TOOL_H
#define LIBREMAPPER_CUTOUT_TOOL_H

#include <memory>

#include <QObject>
#include <QString>

#include "tool_base.h"

class QAction;
class QKeyEvent;
class QPainter;
class QRectF;

namespace OpenOrienteering {

class Map;
class MapEditorController;
class MapWidget;
class ObjectSelector;
class PathObject;


/** Tool to make map cutouts */
class CutoutTool : public MapEditorToolBase
{
Q_OBJECT
public:
	/**
	 * Constructs a CutoutTool.
	 * 
	 * Setting cut_away to true inverts the tool's effect, cutting the
	 * part inside the cut shape away instead of the part outside.
	 */
	CutoutTool(MapEditorController* editor, QAction* tool_action, bool cut_away);
	
	CutoutTool(const CutoutTool&) = delete;
	
	~CutoutTool() override;
	
	
	/**
	 * Applies the tool to cut all selected objects in map, or to all objects
	 * if there is no selection. cutout_object itself must not be selected!
	 * 
	 * Can also be used without constructing a tool object.
	 * 
	 * Setting cut_away to true inverts the effect, cutting the
	 * part inside the cut shape away instead of the part outside.
	 */
	static void apply(Map* map, PathObject* cutout_object, bool cut_away);
	
	
protected:
	void drawImpl(QPainter* painter, MapWidget* widget) override;
	
	bool keyPress(QKeyEvent* event) override;
	
	void finishEditing() override;
	
	void initImpl() override;
	int updateDirtyRectImpl(QRectF& rect) override;
	void updateStatusText() override;
	void objectSelectionChangedImpl() override;
	
	void clickRelease() override;
	void dragStart() override;
	void dragMove() override;
	void dragFinish() override;
	
	
private:
	/** The object which determines the cutout's shape */
	PathObject* cutout_object;
	
	/** Object selection helper */
	std::unique_ptr<ObjectSelector> object_selector;
	
	/**
	 * The index of the cutout object in its map part, so it can be inserted
	 * there again after taking it out. Set to a negative value to indicate
	 * that the object has already been re-added.
	 */
	int cutout_object_index;
	
	/**
	 * If false, the tool removes all objects outside the cutout area,
	 * otherwise it removes all objects inside the cutout area
	 */
	bool cut_away;
};


}  // namespace OpenOrienteering
#endif
