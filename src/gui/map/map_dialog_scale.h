/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_MAP_DIALOG_SCALE_H
#define LIBREMAPPER_MAP_DIALOG_SCALE_H

#include <QDialog>
#include <QObject>

class QSpinBox;
class QCheckBox;
class QPushButton;
class QRadioButton;
class QDoubleSpinBox;
class QWidget;

namespace OpenOrienteering {

class Map;


/**
 * Dialog for scaling the whole map.
 */
class ScaleMapDialog : public QDialog
{
Q_OBJECT
public:
	/** Creates a new ScaleMapDialog. */
	ScaleMapDialog(QWidget* parent, Map* map);
	
private slots:
	void updateWidgets();
	void okClicked();
	
private:
	QSpinBox* scale_edit;
	QRadioButton* center_origin_radio;
	QRadioButton* center_georef_radio;
	QRadioButton* center_other_radio;
	QDoubleSpinBox* other_x_edit;
	QDoubleSpinBox* other_y_edit;
	
	QCheckBox* adjust_symbols_check;
	QCheckBox* adjust_objects_check;
	QCheckBox* adjust_georeferencing_check;
	QCheckBox* adjust_templates_check;
	QPushButton* ok_button;
	
	Map* map;
};


}  // namespace OpenOrienteering

#endif
