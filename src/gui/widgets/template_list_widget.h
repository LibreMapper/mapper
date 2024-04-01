/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TEMPLATE_LIST_WIDGET_H
#define LIBREMAPPER_TEMPLATE_LIST_WIDGET_H

#include <memory>

#include <Qt>
#include <QWidget>
#include <QObject>
#include <QString>

#include "core/map_view.h"

class QAction;
class QBoxLayout;
class QCheckBox;
class QEvent;
class QModelIndex;
class QTableView;
class QToolButton;
class QVariant;

namespace OpenOrienteering {

class Map;
class MapEditorController;
class Template;
class TemplateTableModel;


/**
 * Widget showing the list of templates, including the map layer.
 * 
 * Allows to load templates, set their view properties and reorder them,
 * and do various other actions like adjusting template positions.
 */
class TemplateListWidget : public QWidget
{
	Q_OBJECT
	
public:
	static std::unique_ptr<Template> showOpenTemplateDialog(QWidget* dialog_parent, MapEditorController& controller);
	
	~TemplateListWidget() override;
	
	TemplateListWidget(Map& map, MapView& main_view, MapEditorController& controller, QWidget* parent = nullptr);
	TemplateListWidget(const TemplateListWidget&) = delete;
	TemplateListWidget(TemplateListWidget&&) = delete;
	TemplateListWidget& operator=(const TemplateListWidget&) = delete;
	TemplateListWidget& operator=(TemplateListWidget&&) = delete;
	
signals:
	void currentRowChanged(int row);
	void currentTemplateChanged(const OpenOrienteering::Template* temp);
	void closePositionDockWidget();
	void closeClicked();
	
protected:
	TemplateTableModel* model();
	const TemplateTableModel* model() const { return static_cast<const TemplateTableModel*>(const_cast<TemplateListWidget*>(this)->model()); }
	QVariant data(int row, int column, int role) const;
	void setData(int row, int column, QVariant value, int role);
	Qt::ItemFlags flags(int row, int column) const;
	
	int currentRow() const;
	int posFromRow(int row) const;
	int rowFromPos(int pos) const;
	Template* currentTemplate();
	
protected:
	/**
	 * Reacts to feature visibility changes.
	 * 
	 * @see MapView::visibilityChanged
	 */
	void updateVisibility(OpenOrienteering::MapView::VisibilityFeature feature, bool active, const OpenOrienteering::Template* temp = nullptr);
	
	/**
	 * Updates widget state depending on general template visibility.
	 *
	 * When all templates are hidden, operations on templates are disabled.
	 */
	void updateAllTemplatesHidden();
	
	void setButtonsDirty();
	void updateButtons();
	
	void itemClicked(const QModelIndex& index);
	void itemDoubleClicked(const QModelIndex& index);
	
	/**
	 * When key events for Qt::Key_Space are sent to the template_table,
	 * this will toggle the visibility of the current template.
	 */
	bool eventFilter(QObject* watched, QEvent* event) override;
	
	
// slots:
#if 0
	void newTemplate(QAction* action);
#endif
	
	void openTemplate();
	void deleteTemplate();
	void duplicateTemplate();
	void moveTemplateUp();
	void moveTemplateDown();
	void showHelp();
	
	void moveByHandClicked(bool checked);
	void adjustClicked(bool checked);
	//void groupClicked();
	void positionClicked(bool checked);
	void importClicked();
	void changeGeorefClicked();
	void moreActionClicked(QAction* action);
	void vectorizeClicked();
	
	void templatePositionDockWidgetClosed(OpenOrienteering::Template* temp);
	
	void changeTemplateFile(int pos);
	
	void showOpacitySlider(int row);
	
private:
	Map& map;
	MapView& main_view;
	MapEditorController& controller;
	bool mobile_mode;
	bool buttons_dirty = false;
	
	Template* last_template = nullptr;
	int last_row = -1;
	
	QCheckBox* all_hidden_check;
	QTableView* template_table;
	QBoxLayout* all_templates_layout;
	
	QAction* duplicate_action;
	QAction* move_by_hand_action;
	QAction* position_action;
	QAction* import_action;
	QAction* georef_action;
	QAction* vectorize_action;
	
	// Buttons
	QWidget* list_buttons_group;
	QToolButton* delete_button;
	QToolButton* move_up_button;
	QToolButton* move_down_button;
	QToolButton* move_by_hand_button;
	QToolButton* adjust_button;
	QToolButton* edit_button;
	
	//QToolButton* group_button;
	//QToolButton* more_button;
};


}  // namespace OpenOrienteering

#endif
