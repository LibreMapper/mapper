/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_TEMPLATE_IMAGE_OPEN_DIALOG_H
#define OPENORIENTEERING_TEMPLATE_IMAGE_OPEN_DIALOG_H

#include <QDialog>
#include <QObject>
#include <QString>

class QLineEdit;
class QPushButton;
class QRadioButton;
class QWidget;

namespace OpenOrienteering {

class TemplateImage;


/**
 * Initial setting dialog when opening a raster image as template,
 * asking for how to position the image.
 * 
 * \todo Move this class to separate files.
 */
class TemplateImageOpenDialog : public QDialog
{
Q_OBJECT
public:
	TemplateImageOpenDialog(TemplateImage* templ, QWidget* parent);
	
	double getMpp() const;
	bool isGeorefRadioChecked() const;
	
protected slots:
	void radioClicked();
	void setOpenEnabled();
	void doAccept();
	
private:
	QRadioButton* georef_radio;
	QRadioButton* mpp_radio;
	QRadioButton* dpi_radio;
	QLineEdit* mpp_edit;
	QLineEdit* dpi_edit;
	QLineEdit* scale_edit;
	QPushButton* open_button;
	
	TemplateImage* templ;
};


}  // namespace OpenOrienteering

#endif  // OPENORIENTEERING_TEMPLATE_IMAGE_OPEN_DIALOG_H
