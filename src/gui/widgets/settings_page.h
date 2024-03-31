/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012 Jan Dalheimer (OpenOrienteering)
 * Copyright 2013-2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_SETTINGS_PAGE_H
#define LIBREMAPPER_SETTINGS_PAGE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QWidget>

#include "settings.h"

namespace OpenOrienteering {


/**
 * A widget which serves as a page in the SettingsDialog.
 */
class SettingsPage : public QWidget
{
Q_OBJECT
public:
	/**
	 * Constructs a new settings page.
	 */
	explicit SettingsPage(QWidget* parent = nullptr);
	
	/**
	 * Destructor.
	 */
	~SettingsPage() override;
	
	/**
	 * Returns the title of this page.
	 */
	virtual QString title() const = 0;
	
public slots:	
	/**
	 * This slot is to transfer the input fields to the settings.
	 */
	virtual void apply() = 0;
	
	/**
	 * This slot is to reset all input widgets to the state of the settings.
	 */
	virtual void reset() = 0;

protected:	
	/**
	 * Convenience function for retrieving a setting.
	 */
	static QVariant getSetting(Settings::SettingsEnum setting);
	
	/**
	 * Convenience function for changing a setting.
	 */
	template <class T>
	static void setSetting(Settings::SettingsEnum setting, T value);
	
};



inline
QVariant SettingsPage::getSetting(Settings::SettingsEnum setting)
{
	return Settings::getInstance().getSetting(setting);
}

template <class T>
void SettingsPage::setSetting(Settings::SettingsEnum setting, T value)
{
	Settings::getInstance().setSetting(setting, QVariant{ value });
}


}  // namespace OpenOrienteering

#endif
