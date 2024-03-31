/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012, 2013, 2014, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_COLOR_LIST_WIDGET_H
#define LIBREMAPPER_COLOR_LIST_WIDGET_H

#include <QObject>
#include <QString>
#include <QWidget>

class QAction;
class QIcon;
class QShowEvent;
class QTableWidget;
class QToolButton;

namespace LibreMapper {

class MainWindow;
class Map;
class MapColor;


/**
 * A widget showing the list of map colors and some edit buttons at the bottom.
 * Enables to define new colors and edit or delete existing colors.
 * This widget is used inside a dock widget.
 */
class ColorListWidget : public QWidget
{
Q_OBJECT
public:
	/** Creates a new ColorWidget for a given map and MainWindow. */
	ColorListWidget(Map* map, MainWindow* window, QWidget* parent = nullptr);
	
	/** Destroys the ColorWidget. */
	~ColorListWidget() override;
	
protected slots:
	void newColor();
	void deleteColor();
	void duplicateColor();
	void moveColorUp();
	void moveColorDown();
	void editCurrentColor();
	void showHelp() const;
	
	void cellChange(int row, int column);
	void currentCellChange(int current_row, int current_column, int previous_row, int previous_column);
	
	void colorAdded(int index, const LibreMapper::MapColor* color);
	void colorChanged(int index, const LibreMapper::MapColor* color);
	void colorDeleted(int index, const LibreMapper::MapColor* color);
	
protected:
	void showEvent(QShowEvent* event) override;
	
private:
	void addRow(int row);
	void updateRow(int row);
	
	// Color list
	QTableWidget* color_table;
	
	QAction* duplicate_action;
	
	// Buttons
	QToolButton* delete_button;
	QToolButton* move_up_button;
	QToolButton* move_down_button;
	QToolButton* edit_button;
	
	Map* const map;
	MainWindow* const window;
	bool react_to_changes;
};


}  // namespace LibreMapper

#endif
