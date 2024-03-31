/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012 Thomas Schöps (OpenOrienteering)
 * Copyright 2013, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "template_positioning_dialog.h"

#include <Qt>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFlags>
#include <QFormLayout>
#include <QRadioButton>
#include <QSpacerItem>

#include "gui/util_gui.h"


namespace LibreMapper {

TemplatePositioningDialog::TemplatePositioningDialog(QWidget* parent)
: QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
	setWindowModality(Qt::WindowModal);
	setWindowTitle(tr("Track scaling and positioning"));
	
	QFormLayout* layout = new QFormLayout();
	
	coord_system_box = new QComboBox();
	layout->addRow(tr("Coordinate system"), coord_system_box);
	coord_system_box->addItem(tr("Real"));
	coord_system_box->addItem(tr("Map"));
	coord_system_box->setCurrentIndex(0);
	
	unit_scale_edit = Util::SpinBox::create(6, 0, 99999.999999, tr("m", "meters"));
	unit_scale_edit->setValue(1);
	unit_scale_edit->setEnabled(false);
	layout->addRow(tr("One coordinate unit equals:"), unit_scale_edit);
	
	original_pos_radio = new QRadioButton(tr("Position track at given coordinates"));
	original_pos_radio->setChecked(true);
	layout->addRow(original_pos_radio);
	
	view_center_radio = new QRadioButton(tr("Position track at view center"));
	layout->addRow(view_center_radio);
	
	layout->addItem(Util::SpacerItem::create(this));
	
	auto button_box = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	layout->addWidget(button_box);
	
	setLayout(layout);
	
	connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool TemplatePositioningDialog::useRealCoords() const
{
	return coord_system_box->currentIndex() == 0;
}

double TemplatePositioningDialog::getUnitScale() const
{
	return unit_scale_edit->value();
}

bool TemplatePositioningDialog::centerOnView() const
{
	return view_center_radio->isChecked();
}


}  // namespace LibreMapper
