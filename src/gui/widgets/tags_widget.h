/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TAGS_WIDGET_H
#define LIBREMAPPER_TAGS_WIDGET_H

#include <QObject>
#include <QString>
#include <QWidget>

class QIcon;
class QTableWidget;
class QToolButton;

namespace LibreMapper {

class Map;
class MapEditorController;
class MapView;
class Object;


/**
 * This widget allows for display and editing of tags, i.e. key-value pairs.
 */
class TagsWidget : public QWidget
{
Q_OBJECT
public:
	/** Constructs a new tags widget for the given map. */
	explicit TagsWidget(Map* map, MapView* main_view, MapEditorController* controller, QWidget* parent = nullptr);
	
	/** Destroys the widget. */
	~TagsWidget() override;
	
protected slots:
	/**
	 * Updates the widget from the current object's tags.
	 * 
	 * Does nothing if react_to_changes is set to false.
	 */
	void objectTagsChanged();
	
	/**
	 * Updates the current object's tags from the widget.
	 * 
	 * Does nothing if react_to_changes is set to false.
	 */
	void cellChange(int row, int column);
	
	/**
	 * Opens the help page for this widget.
	 */
	void showHelp();
	
protected:
	/** 
	 * Sets up the last row: blank cells, right one not editable.
	 */
	void setupLastRow();
	
	/**
	 * Creates and registers an undo step for the given object.
	 * 
	 * @param object must not be null.
	 */
	void createUndoStep(Object* object);
	
private:
	Map* map;
	MapView* main_view;
	MapEditorController* controller;
	bool react_to_changes;
	
	QTableWidget* tags_table;
};


}  // namespace LibreMapper

#endif // TAGS_WIDGET_H
