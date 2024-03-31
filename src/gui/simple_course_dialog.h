/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2021 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_SIMPLE_COURSE_DIALOG_H
#define LIBREMAPPER_SIMPLE_COURSE_DIALOG_H

#include <QtGlobal>
#include <QDialog>
#include <QObject>
#include <QString>

class QDialogButtonBox;
class QLineEdit;
class QSpinBox;
class QWidget;

namespace LibreMapper {

class SimpleCourseExport;


/**
 * A dialog to provide extra information for simple course export.
 */
class SimpleCourseDialog : public QDialog
{
Q_OBJECT
public:
	~SimpleCourseDialog() override;
	
	SimpleCourseDialog(const SimpleCourseExport& simple_course, QWidget* parent);
	
	QString eventName() const;
	
	QString courseName() const;
	
	int firstCodeNumber() const;
	
protected:
	void updateWidgets();
	
private:
	const SimpleCourseExport& simple_course;
	QLineEdit* event_name_edit;
	QLineEdit* course_name_edit;
	QSpinBox*  first_code_spinbox;
	QDialogButtonBox* button_box;
	
	Q_DISABLE_COPY(SimpleCourseDialog)
};


}  // namespace LibreMapper

#endif  // LIBREMAPPER_SIMPLE_COURSE_DIALOG_H
