/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2019, 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_STRETCH_MAP_DIALOG_H
#define LIBREMAPPER_STRETCH_MAP_DIALOG_H

#include <functional>

#include <Qt>
#include <QtGlobal>
#include <QDialog>
#include <QObject>
#include <QString>

class QCheckBox;
class QDoubleSpinBox;
class QPushButton;
class QRadioButton;
class QWidget;

namespace LibreMapper {

class Map;


/**
 * Dialog for stretching or shrinking the whole map.
 */
class StretchMapDialog : public QDialog
{
Q_OBJECT
public:
	using StretchOp = std::function<void (Map&)>;
	
	/** Creates a new StretchMapDialog. */
	StretchMapDialog(const Map& map, double stretch_factor, QWidget* parent = nullptr, Qt::WindowFlags f = {});
	
	/** Performs the configured scaling on the given map. */
	void stretch(Map& map) const;
	
	/** Returns a scaling functor. */
	Q_REQUIRED_RESULT StretchOp makeStretch() const;
	
private slots:
	void updateWidgets();
	
private:
	QRadioButton* center_origin_radio;
	QRadioButton* center_georef_radio;
	QRadioButton* center_other_radio;
	QDoubleSpinBox* other_x_edit;
	QDoubleSpinBox* other_y_edit;
	
	QCheckBox* adjust_georeferencing_check;
	QCheckBox* adjust_templates_check;
	QPushButton* ok_button;
	
	const double stretch_factor;
};


}  // namespace LibreMapper

#endif
