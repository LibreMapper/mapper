/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */

#include "ocd_compat_settings_page.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QString>
#include <QVariant>

#include "settings.h"
#include "fileformats/ocd_file_import.h"
#include "gui/util_gui.h"
#include "gui/widgets/settings_page.h"

class QWidget;

namespace LibreMapper {

OcdCompatSettingsPage::OcdCompatSettingsPage(QWidget* parent)
    : SettingsPage(parent)
{
	auto* layout = new QFormLayout(this);

	layout->addRow(Util::Headline::create(tr("File handling")));

	leave_paths_open = new QCheckBox(::LibreMapper::OcdFileImport::tr("Leave paths (lines and area borders) open on file import"));
	layout->addRow(leave_paths_open);

	updateWidgets();
}

OcdCompatSettingsPage::~OcdCompatSettingsPage() = default;

QString OcdCompatSettingsPage::title() const
{
	return tr("OCD format");
}

void OcdCompatSettingsPage::apply()
{
	setSetting(Settings::OcdCompatLeavePathsOpenOnImport, leave_paths_open->isChecked());
}

void OcdCompatSettingsPage::reset()
{
	updateWidgets();
}

void OcdCompatSettingsPage::updateWidgets()
{
	leave_paths_open->setChecked(getSetting(Settings::HomeScreen_TipsVisible).toBool());
}


}  // namespace LibreMapper
