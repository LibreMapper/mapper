/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013, 2014, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_COLOR_DIALOG_H
#define OPENORIENTEERING_COLOR_DIALOG_H

#include <vector>

#include <Qt>
#include <QDialog>
#include <QObject>

#include "core/map_color.h"

class QAbstractButton;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QRadioButton;
class QTabWidget;
class QWidget;

namespace OpenOrienteering {

class ColorDropDown;
class Map;


/**
 * A dialog for editing a single map color.
 */
class ColorDialog: public QDialog
{
Q_OBJECT
public:
	/** Constructs a new dialog for the given map and color. */
	ColorDialog(const Map& map, const MapColor& source_color, QWidget* parent = nullptr, Qt::WindowFlags f = {});
	
	~ColorDialog() override;
	
	/**
	 * Returns the edited color. 
	 */
	const MapColor& getColor() const { return color; }
	
protected slots:
	void accept() override;
	
	void reset();
	
	void showHelp();
	
	void languageChanged();
	
	void editClicked();
	
	void mapColorNameChanged();
	
	void spotColorTypeChanged(int id);
	
	void spotColorNameChanged();
	
	void spotColorScreenChanged();
	
	void spotColorCompositionChanged();
	
	void knockoutChanged();
	
	void cmykColorTypeChanged(int id);
	
	void cmykValueChanged();
	
	void rgbColorTypeChanged(int id);
	
	void rgbValueChanged();
	
	void setColorModified() { setColorModified(true); }
	
protected:
	void setColorModified(bool modified);
	
	void updateColorLabel();
	
	void updateWidgets();
	
	void updateButtons();
	
	const Map& map;
	const MapColor& source_color;
	
	MapColor color;
	bool color_modified;
	
	bool react_to_changes;
	
	QLabel* color_preview_label;
	QLabel* color_name_label;
	QLineEdit* mc_name_edit;
	QComboBox* language_combo;
	QPushButton* name_edit_button;
	
	QRadioButton* full_tone_option;
	QRadioButton* composition_option;
	QLineEdit* sc_name_edit;
	QDoubleSpinBox* sc_frequency_edit;
	QDoubleSpinBox* sc_angle_edit;
	QCheckBox* knockout_option;
	
	QRadioButton* cmyk_spot_color_option;
	QRadioButton* evaluate_rgb_option;
	QRadioButton* custom_cmyk_option;
	QDoubleSpinBox* c_edit;
	QDoubleSpinBox* m_edit;
	QDoubleSpinBox* y_edit;
	QDoubleSpinBox* k_edit;
	
	QRadioButton* rgb_spot_color_option;
	QRadioButton* evaluate_cmyk_option;
	QRadioButton* custom_rgb_option;
	QDoubleSpinBox* r_edit;
	QDoubleSpinBox* g_edit;
	QDoubleSpinBox* b_edit;
	QLineEdit* html_edit;
	
	QTabWidget* properties_widget;
	
	QListWidget* color_usage_list;
	
	QAbstractButton* ok_button;
	QAbstractButton* reset_button;
	
	std::vector< ColorDropDown* > component_colors;
	std::vector< QDoubleSpinBox* > component_halftone;
	int components_row0;
	int components_col0;
	QGridLayout* prof_color_layout;
	int stretch_row0;
	int stretch_col0;
	QWidget* stretch;
};


}  // namespace OpenOrienteering

#endif
