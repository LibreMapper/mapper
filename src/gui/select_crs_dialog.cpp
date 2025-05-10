/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "select_crs_dialog.h"

#include <Qt>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLatin1String>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "core/crs_template.h"
#include "core/georeferencing.h"
#include "gui/util_gui.h"
#include "gui/widgets/crs_selector.h"


namespace LibreMapper {

namespace {

enum SpecialCRS {
	SameAsMap    = 1,
	Local        = 2,
	Geographic   = 3,
	TemplateFile = 4,
};


}  // namespace



SelectCRSDialog::SelectCRSDialog(
        const TemplateImage::GeoreferencingOptions& options,
        const Georeferencing& georef,
        QWidget* parent,
        const QString& description )
 : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
 , options(options)
 , georef(georef)
{
	setWindowModality(Qt::WindowModal);
	setWindowTitle(tr("Select coordinate reference system"));
	
	crs_selector = new CRSSelector(georef, nullptr);
	if (georef.getState() != Georeferencing::Geospatial)
	{
		crs_selector->clear();
		crs_selector->addCustomItem(tr("Local"), SpecialCRS::Local);
	}
	else
	{
		if (!options.template_file.crs_spec.isEmpty())
			crs_selector->addCustomItem(tr("From template file"), SpecialCRS::TemplateFile );
		crs_selector->addCustomItem(tr("Same as map"), SpecialCRS::SameAsMap);
		crs_selector->addCustomItem(tr("Geographic coordinates (WGS84)"), SpecialCRS::Geographic);
	}
	
	status_label = new QLabel();
	button_box = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	
	auto form_layout = new QFormLayout();
	if (!description.isEmpty())
	{
		form_layout->addRow(new QLabel(description));
		form_layout->addItem(Util::SpacerItem::create(this));
	}
	form_layout->addRow(QCoreApplication::translate("LibreMapper::GeoreferencingDialog", "&Coordinate reference system:"), crs_selector);
	form_layout->addRow(tr("Status:"), status_label);
	form_layout->addItem(Util::SpacerItem::create(this));
	crs_selector->setDialogLayout(form_layout);
	
	auto const& crs_spec = options.effective.crs_spec;
	if (georef.getState() != Georeferencing::Geospatial)
	    crs_selector->setCurrentIndex(0);
	else if (crs_spec.isEmpty())
		crs_selector->setCurrentIndex(crs_selector->findData(SpecialCRS::SameAsMap));
	else if (crs_spec == options.template_file.crs_spec)
		crs_selector->setCurrentIndex(crs_selector->findData(SpecialCRS::TemplateFile));
	else if (crs_spec == georef.getProjectedCRSSpec())
		crs_selector->setCurrentIndex(crs_selector->findData(SpecialCRS::SameAsMap));
	else if (crs_spec == Georeferencing::geographic_crs_spec)
		crs_selector->setCurrentIndex(crs_selector->findData(SpecialCRS::Geographic));
	else
		crs_selector->setCurrentCRS(CRSTemplateRegistry().find(QString::fromLatin1("PROJ.4")), { crs_spec });
	
	auto layout = new QVBoxLayout();
	layout->addLayout(form_layout, 1);
	layout->addWidget(button_box, 0);
	setLayout(layout);
	
	connect(crs_selector, &CRSSelector::crsChanged, this, &SelectCRSDialog::updateWidgets);
	connect(button_box, &QDialogButtonBox::accepted, this, &SelectCRSDialog::accept);
	connect(button_box, &QDialogButtonBox::rejected, this, &SelectCRSDialog::reject);
	
	updateWidgets();
}

QString SelectCRSDialog::currentCRSSpec() const
{
	QString spec;
	switch (crs_selector->currentCustomItem())
	{
	case SpecialCRS::SameAsMap:
		spec = georef.getProjectedCRSSpec();
		break;
	case SpecialCRS::Local:
		// nothing
		break;
	case SpecialCRS::Geographic:
		spec = Georeferencing::geographic_crs_spec;
		break;
	case SpecialCRS::TemplateFile:
		spec = options.template_file.crs_spec;
		break;
	default:
		spec = crs_selector->currentCRSSpec();
	}

	return spec;
}

void SelectCRSDialog::updateWidgets()
{
	Georeferencing georef;
	auto spec =  currentCRSSpec();
	auto valid = spec.isEmpty() || georef.setProjectedCRS({}, spec);
	
	button_box->button(QDialogButtonBox::Ok)->setEnabled(valid);
	if (valid)
		status_label->setText(tr("valid"));
	else
		status_label->setText(QLatin1String("<b style=\"color:red\">") + georef.getErrorText() + QLatin1String("</b>"));
}


}  // namespace LibreMapper
