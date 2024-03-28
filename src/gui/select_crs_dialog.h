/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_SELECT_CRS_DIALOG_H
#define OPENORIENTEERING_SELECT_CRS_DIALOG_H

#include <QDialog>
#include <QFlags>
#include <QObject>
#include <QString>

#include "templates/template_image.h"

class QDialogButtonBox;
class QLabel;
class QWidget;

namespace OpenOrienteering {

class CRSSelector;
class Georeferencing;


/** Dialog to select a coordinate reference system (CRS) */
class SelectCRSDialog : public QDialog
{
Q_OBJECT
public:
	/**
	 * Creates a SelectCRSDialog.
	 * 
	 * @param georef       A default georeferencing (usually the map's one).
	 * @param parent       The parent widget.
	 * @param alternatives The georeferencing alternatives to be offered.
	 * @param description  Optional description text for the dialog.
	 *                     Should explain what the selected CRS will be used for.
	 */
	SelectCRSDialog(
	        const TemplateImage::GeoreferencingOptions& options,
	        const Georeferencing& georef,
	        QWidget* parent,
	        const QString& description = QString()
	);
	
	/** 
	 * Returns the current CRS specification string.
	 */
	QString currentCRSSpec() const;
	
protected:
	/** 
	 * Update the status field and enables/disables the OK button.
	 */
	void updateWidgets();
	
private:
	const TemplateImage::GeoreferencingOptions& options;
	const Georeferencing& georef;
	CRSSelector* crs_selector;
	QLabel* status_label;
	QDialogButtonBox* button_box;
};


}  // namespace OpenOrienteering

#endif
