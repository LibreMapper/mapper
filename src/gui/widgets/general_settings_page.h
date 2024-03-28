/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Jan Dalheimer (OpenOrienteering)
 * Copyright 2013-2016  Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_GENERAL_SETTINGS_PAGE_H
#define OPENORIENTEERING_GENERAL_SETTINGS_PAGE_H

#include <QObject>
#include <QString>
#include <QVariant>

#include "settings_page.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QEvent;
class QSpinBox;
class QWidget;


namespace OpenOrienteering {

class GeneralSettingsPage : public SettingsPage
{
Q_OBJECT
public:
	explicit GeneralSettingsPage(QWidget* parent = nullptr);
	
	~GeneralSettingsPage() override;
	
	QString title() const override;
	
	void apply() override;
	
	void reset() override;
	
protected:
	/**
	 * Adds the available languages to the language combo box,
	 * and sets the current element.
	 */
	void updateLanguageBox(QVariant code);
	
	void updateWidgets();
	
	/**
	 * This event filter stops LanguageChange events.
	 */
	bool eventFilter(QObject* watched, QEvent* event) override;
	
private slots:
	void openTranslationFileDialog();
	
	void openPPICalculationDialog();
	
	void encodingChanged(const QString& input);
	
private:
	QString    translation_file;
	QString    last_encoding_input;
	QString    last_matching_completition;
	
	QComboBox* language_box;
	
	QDoubleSpinBox* ppi_edit;
	QCheckBox* open_mru_check;
	QCheckBox* tips_visible_check;
	
	QCheckBox* compatibility_check;
	QCheckBox* undo_check;
	QCheckBox* autosave_check;
	QSpinBox*  autosave_interval_edit;
	
	QComboBox* encoding_box;
};


}  // namespace OpenOrienteering

#endif
