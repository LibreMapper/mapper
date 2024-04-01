/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2021 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "simple_course_dialog.h"

#include <Qt>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

#include "gui/util_gui.h"
#include "fileformats/simple_course_export.h"


namespace OpenOrienteering {

SimpleCourseDialog::~SimpleCourseDialog() = default;

SimpleCourseDialog::SimpleCourseDialog(const SimpleCourseExport& simple_course, QWidget* parent)
: QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
, simple_course{simple_course}
{
	setWindowModality(Qt::WindowModal);
	setWindowTitle(tr("Event and course details"));
	
	auto* form_layout = new QFormLayout();
	
	event_name_edit = new QLineEdit();
	form_layout->addRow(tr("Event name:"), event_name_edit);
	
	course_name_edit = new QLineEdit();
	form_layout->addRow(tr("Course name:"), course_name_edit);
	
	first_code_spinbox = Util::SpinBox::create(1, 901);
	form_layout->addRow(tr("First code number:"), first_code_spinbox);
	
	button_box = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	
	auto layout = new QVBoxLayout();
	layout->addLayout(form_layout, 1);
	layout->addWidget(button_box, 0);
	setLayout(layout);
	
	connect(button_box, &QDialogButtonBox::accepted, this, &SimpleCourseDialog::accept);
	connect(button_box, &QDialogButtonBox::rejected, this, &SimpleCourseDialog::reject);
	updateWidgets();
}


QString SimpleCourseDialog::eventName() const
{
	return event_name_edit->text();
}

QString SimpleCourseDialog::courseName() const
{
	return course_name_edit->text();
}

int SimpleCourseDialog::firstCodeNumber() const
{
	return first_code_spinbox->value();
}


void SimpleCourseDialog::updateWidgets()
{
	event_name_edit->setText(simple_course.eventName());
	course_name_edit->setText(simple_course.courseName());
	first_code_spinbox->setValue(simple_course.firstCode());
}


}  // namespace OpenOrienteering
