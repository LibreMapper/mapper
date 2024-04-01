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
, current_tip(-1)
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
	
	Settings::getInstance().setSetting(Settings::HomeScreen_CurrentTip, current_tip);
}

void HomeScreenController::readSettings()
{
	Settings& settings = Settings::getInstance(); // FIXME: settings should be const
	
	widget->setRecentFiles(settings.getSettingCached(Settings::General_RecentFilesList).toStringList());
	widget->setOpenMRUFileChecked(settings.getSettingCached(Settings::General_OpenMRUFile).toBool());
	
	bool tips_visible = settings.getSettingCached(Settings::HomeScreen_TipsVisible).toBool();
	widget->setTipsVisible(tips_visible);
	if (tips_visible)
	{
		if (current_tip < 0)
		{
			current_tip = settings.getSettingCached(Settings::HomeScreen_CurrentTip).toInt();
			goToNextTip();
		}
		else
		{
			// Settings changed.
			goToTip(current_tip);
		}
	}
}

void HomeScreenController::setOpenMRUFile(bool state)
{
	Settings::getInstance().setSetting(Settings::General_OpenMRUFile, state);
}

void HomeScreenController::clearRecentFiles()
{
	Settings::getInstance().remove(Settings::General_RecentFilesList);
}

void HomeScreenController::setTipsVisible(bool state)
{
	Settings::getInstance().setSetting(Settings::HomeScreen_TipsVisible, state);
}

void HomeScreenController::goToNextTip()
{
	goToTip(current_tip + 1);
}

void HomeScreenController::goToPreviousTip()
{
	goToTip(current_tip - 1);
}

void HomeScreenController::goToTip(int index)
{
	static QStringList tips;
	if (tips.isEmpty())
	{
		// Normally, this will be read only once.
		QFile file(QString::fromLatin1("doc:tip-of-the-day/tips.txt"));
		if (file.open(QIODevice::ReadOnly))
		{
			while (!file.atEnd())
			{
				QString tip(QString::fromUtf8(file.readLine().constData()));
				if (tip.endsWith(QLatin1Char('\n')))
					tip.chop(1);
				if (!tip.isEmpty())
					tips.push_back(tip);
			}
		}
	}
	
	if (tips.isEmpty())
	{
		// Some error may have occurred during reading the tips file.
		// Display a welcome text.
		widget->setTipOfTheDay(QString::fromLatin1("<h2>%1</h2>").arg(tr("Welcome to OpenOrienteering Mapper!")));
	}
	else
	{
		Q_ASSERT(tips.count() > 0);
		while (index < 0)
			index += tips.count();
		current_tip = index % tips.count();
		widget->setTipOfTheDay(tips[current_tip]);
	}
}


}  // namespace LibreMapper
