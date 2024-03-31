/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012 Thomas Schöps (OpenOrienteering)
 * Copyright 2013, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TEMPLATE_POSITIIONING_DIALOG_H
#define LIBREMAPPER_TEMPLATE_POSITIIONING_DIALOG_H

#include <QtGlobal>
#include <QDialog>
#include <QObject>

class QComboBox;
class QDoubleSpinBox;
class QRadioButton;
class QWidget;

namespace LibreMapper {


/**
 * Dialog allowing for positioning of a template.
 */
class TemplatePositioningDialog : public QDialog
{
Q_OBJECT
public:
	TemplatePositioningDialog(QWidget* parent = nullptr);
	
	bool useRealCoords() const;
	double getUnitScale() const;
	bool centerOnView() const;
	
private:
	QComboBox* coord_system_box;
	QDoubleSpinBox* unit_scale_edit;
	QRadioButton* original_pos_radio;
	QRadioButton* view_center_radio;
	
	Q_DISABLE_COPY(TemplatePositioningDialog)
};


}  // namespace LibreMapper

#endif
