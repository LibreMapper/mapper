/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_OCD_COMPAT_SETTINGS_PAGE_H
#define LIBREMAPPER_OCD_COMPAT_SETTINGS_PAGE_H

#include <QObject>

#include "gui/widgets/settings_page.h"

class QCheckBox;
class QWidget;


namespace LibreMapper {

class OcdCompatSettingsPage : public SettingsPage
{
	Q_OBJECT
public:
	explicit OcdCompatSettingsPage(QWidget* parent = nullptr);
	~OcdCompatSettingsPage() override;
	QString title() const override;
	void apply() override;
	void reset() override;

protected:
	void updateWidgets();
	
private:
	QCheckBox* leave_paths_open;
};


}  // namespace LibreMapper

#endif
