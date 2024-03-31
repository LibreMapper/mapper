/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_CRS_SELECTOR_H
#define LIBREMAPPER_CRS_SELECTOR_H

#include <vector>

#include <QComboBox>
#include <QObject>
#include <QString>

#include "core/crs_template.h"

class QEvent;
class QFormLayout;
class QWidget;

namespace OpenOrienteering {

class Georeferencing;


/** 
 * Combobox for selecting projected coordinate reference system (CRS).
 * 
 * It operates on the list of CRS templates in the CRSTemplateRegistry.
 * However, it accepts custom items which are prepended to and separated from
 * the default items.
 * 
 * This is more than just a simple widget. CRSSelector is meant to be inserted
 * into a QFormLayout. Upon CRS selection changes it will add and remove extra
 * lines below itself, for editing CRS parameters.
 * 
 * \todo Consider making this a QWidget which has got a QCombobox - the public
 *       QComboBox API should better not be exposed. The combobox' signals could
 *       no longer be block from clients.
 */
class CRSSelector : public QComboBox, public CRSParameterWidgetObserver
{
Q_OBJECT
public:
	/** 
	 * Constructor.
	 *
	 * The dialog parameter must not be nullptr. It is passed to parameter widgets.
	 * Ownership is taken only by the parent widget if given.
	 */
	CRSSelector(const Georeferencing& georef, QWidget* parent = nullptr);
	
	/**
	 * Destructor.
	 */
	~CRSSelector() override;
	
	
	/**
	 * Sets the QFormLayout which this field is part of.
	 * 
	 * When the selected CRS is changed, or when configureParameterFields()
	 * is called explicitly, CRSSelector will add extra lines below its own row
	 * for editing CRS parameters.
	 * 
	 * This is to be called once. However, it will not create the parameter
	 * fields for the current selection.
	 */
	void setDialogLayout(QFormLayout* dialog_layout);
	
	
	/** 
	 * Adds a custom item with the given text and id at the top of the list.
	 */
	void addCustomItem(const QString& text, unsigned short id);
	
	
	/** 
	 * Returns the selected CRS template, or nullptr if a custom item is selected.
	 */
	const CRSTemplate* currentCRSTemplate() const;
	
	/** 
	 * Returns the selected CRS specification string,
	 * or an empty string if a custom item is selected.
	 */
	QString currentCRSSpec() const;
	
	/** 
	 * Returns the id of the selected custom item,
	 * or -1 if a normal item is selected.
	 */
	int currentCustomItem() const;
	

	/**
	 *  Selects the given standard item, and sets the parameters.
	 */
	void setCurrentCRS(const CRSTemplate* crs, const std::vector<QString>& values);
	
	/** 
	 * Selects the given custom item.
	 */
	void setCurrentItem(unsigned short id);
	
	
	/**
	 * Returns the list of CRS configuration parameter values.
	 */
	std::vector<QString> parameters() const;
	
	
	/** 
	 * Provides the current georeferencing.
	 */
	const Georeferencing& georeferencing() const override;
	
	
signals:
	/** 
	 * Emitted when the user changes the CRS or its parameters.
	 */
	void crsChanged();
	
	
protected:
	/**
	 * Listens to changes of the selected CRS.
	 */
	void crsSelectionChanged();
	
	/**
	 * Listens to changes of CRS parameters.
	 */
	void crsParameterEdited() override;
	
	/**
	 * Updates the parameter fields in the dialog_layout,
	 * according to the selected CRS.
	 */
	void configureParameterFields();
	
	/**
	 * Updates the parameter fields in the dialog_layout,
	 * according to the given CRS and values.
	 */
	void configureParameterFields(const CRSTemplate* crs, const std::vector<QString>& values);
		
	/**
	 * Adds parameter fields to the dialog_layout,
	 * according to the given crs.
	 * 
	 * There must be no other parameter fields in the dialog,
	 * i.e. removeParameterFields() needs to be called before.
	 */
	void addParameterFields(const CRSTemplate* crs);
	
	/**
	 * Removes all parameter fields from the dialog_layout.
	 */
	void removeParameterFields();
	
	/**
	 * Propagates enabling/disabling to the parameter widgets.
	 */
	void changeEvent(QEvent* event) override;
	
private:
	const Georeferencing& georef;
	QFormLayout* dialog_layout;
	int num_custom_items;
	const CRSTemplate* configured_crs;
};


}  // namespace OpenOrienteering

#endif
