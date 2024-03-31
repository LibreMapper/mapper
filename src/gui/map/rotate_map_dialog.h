/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_ROTATE_MAP_DIALOG_H
#define LIBREMAPPER_ROTATE_MAP_DIALOG_H

#include <functional>

#include <Qt>
#include <QtGlobal>
#include <QDialog>
#include <QObject>
#include <QString>

class QDoubleSpinBox;
class QCheckBox;
class QRadioButton;
class QWidget;

namespace LibreMapper {

class Map;


/**
 * Dialog for rotating the whole map around a point.
 */
class RotateMapDialog : public QDialog
{
Q_OBJECT
public:
	using RotationOp = std::function<void (Map&)>;
	
	/** Creates a new RotateMapDialog. */
	RotateMapDialog(const Map& map, QWidget* parent = nullptr, Qt::WindowFlags f = {});
	
	/** Sets the rotation angle in degrees in the corresponding widget. */
	void setRotationDegrees(double rotation);
	/** Enables the setting to rotate around the georeferencing reference point. */
	void setRotateAroundGeorefRefPoint();
	/** Checks or unchecks the setting to adjust the georeferencing declination. */
	void setAdjustDeclination(bool adjust);
	/** Sets the visibility of the setting to adjust the georeferencing declination. */
	void showAdjustDeclination(bool show);
	
	/** Performs the configured rotation on the given map. */
	void rotate(Map& map) const;
	
	/** Returns a rotation functor. */
	Q_REQUIRED_RESULT RotationOp makeRotation() const;
	
private slots:
	void updateWidgets();
	
private:
	QDoubleSpinBox* rotation_edit;
	QRadioButton* center_origin_radio;
	QRadioButton* center_georef_radio;
	QRadioButton* center_other_radio;
	QDoubleSpinBox* other_x_edit;
	QDoubleSpinBox* other_y_edit;
	
	QCheckBox* adjust_georeferencing_check;
	QCheckBox* adjust_declination_check;
	QCheckBox* adjust_templates_check;
};


}  // namespace LibreMapper

#endif
