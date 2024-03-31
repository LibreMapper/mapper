/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps, Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "home_screen_controller.h"

#include <QFile>
#include <QStatusBar>

#include "settings.h"
#include "gui/main_window.h"
#include "gui/widgets/home_screen_widget.h"


namespace LibreMapper {

HomeScreenController::HomeScreenController()
: widget(nullptr)
{
	// nothing
}

HomeScreenController::~HomeScreenController() = default;


bool HomeScreenController::statusBarVisible()
{
	return false;
}

void HomeScreenController::attach(MainWindow* window)
{
	this->window = window;
	
	if (Settings::mobileModeEnforced())
	{
		widget = new HomeScreenWidgetMobile(this);
	}
	else
	{
		widget = new HomeScreenWidgetDesktop(this);
	}
	
	window->setCentralWidget(widget);
	
	connect(&Settings::getInstance(), &Settings::settingsChanged, this, &HomeScreenController::readSettings);
	
	readSettings();
}

void HomeScreenController::detach()
{
	window->setCentralWidget(nullptr);
	widget->deleteLater();
}

void HomeScreenController::readSettings()
{
	Settings& settings = Settings::getInstance(); // FIXME: settings should be const
	
	widget->setRecentFiles(settings.getSettingCached(Settings::General_RecentFilesList).toStringList());
	widget->setOpenMRUFileChecked(settings.getSettingCached(Settings::General_OpenMRUFile).toBool());
}

void HomeScreenController::setOpenMRUFile(bool state)
{
	Settings::getInstance().setSetting(Settings::General_OpenMRUFile, state);
}

void HomeScreenController::clearRecentFiles()
{
	Settings::getInstance().remove(Settings::General_RecentFilesList);
}

}  // namespace LibreMapper
