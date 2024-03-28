/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_TEMPLATE_POSITION_DOCK_WIDGET_H
#define OPENORIENTEERING_TEMPLATE_POSITION_DOCK_WIDGET_H

#include <QDockWidget>
#include <QObject>

class QEvent;
class QCloseEvent;
class QLineEdit;
class QWidget;

namespace OpenOrienteering {

class MapEditorController;
class Template;


/** Dock widget allowing to enter template positioning properties numerically. */
class TemplatePositionDockWidget : public QDockWidget
{
Q_OBJECT
public:
	TemplatePositionDockWidget(Template* temp, MapEditorController* controller, QWidget* parent = nullptr);
	
	void updateValues();
	
	bool event(QEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	
signals:
	void closed();
	
public slots:
	void templateChanged(int index, const OpenOrienteering::Template* temp);
	void templateDeleted(int index, const OpenOrienteering::Template* temp);
	void valueChanged();
	
private:
	QLineEdit* x_edit;
	QLineEdit* y_edit;
	QLineEdit* scale_x_edit;
	QLineEdit* scale_y_edit;
	QLineEdit* rotation_edit;
	QLineEdit* shear_edit;
	
	bool react_to_changes;
	Template* temp;
	MapEditorController* controller;
};


}  // namespace OpenOrienteering

#endif
