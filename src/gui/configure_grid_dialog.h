/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014, 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_CONFIGURE_GRID_DIALOG_H
#define LIBREMAPPER_CONFIGURE_GRID_DIALOG_H

#include <QDialog>
#include <QObject>
#include <QRgb>
#include <QString>

#include "core/map_grid.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QWidget;

namespace OpenOrienteering {

class Map;


class ConfigureGridDialog : public QDialog
{
Q_OBJECT
public:
	ConfigureGridDialog(QWidget* parent, const Map& map, bool grid_visible);
	
	~ConfigureGridDialog() override;
	
	const MapGrid& resultGrid() const;
	
	bool gridVisible() const;
	
private slots:
	void chooseColor();
	void updateColorDisplay();
	void unitChanged(int index);
	void okClicked();
	void updateStates();
	void showHelp();
	
private:
	QCheckBox* show_grid_check;
	QCheckBox* snap_to_grid_check;
	QPushButton* choose_color_button;
	QComboBox* display_mode_combo;

	QRadioButton* mag_north_radio;
	QRadioButton* grid_north_radio;
	QRadioButton* true_north_radio;
	QDoubleSpinBox* additional_rotation_edit;
	
	QComboBox* unit_combo;
	QDoubleSpinBox* horz_spacing_edit;
	QDoubleSpinBox* vert_spacing_edit;
	QLabel* origin_label;
	QDoubleSpinBox* horz_offset_edit;
	QDoubleSpinBox* vert_offset_edit;
	
	const Map& map;
	MapGrid grid;
	bool grid_visible;
	QRgb current_color;
	MapGrid::Unit current_unit;
};

inline const MapGrid& ConfigureGridDialog::resultGrid() const
{
	return grid;
}

inline bool ConfigureGridDialog::gridVisible() const
{
	return grid_visible;
}


}  // namespace OpenOrienteering

#endif
