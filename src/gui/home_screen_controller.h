/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps, Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_HOME_SCREEN_CONTROLLER_H
#define LIBREMAPPER_HOME_SCREEN_CONTROLLER_H

#include "main_window_controller.h"

#include <QObject>

namespace LibreMapper {

class AbstractHomeScreenWidget;
class MainWindow;


/**
 * The controller of the OpenOrienteering Mapper home screen.
 * The OpenOrienteering Mapper home screen is shown when no document is open,
 * for example after the program is started for the first time.
 */
class HomeScreenController : public MainWindowController
{
Q_OBJECT
public:
	/** Creates a new HomeScreenController. */
	HomeScreenController();
	
	/** Destroys the HomeScreenController and its children. */
	~HomeScreenController() override;
	
	/** The home screen widgets do not use a status bar. */
	bool statusBarVisible() override;
	
	/** Activates the HomeScreenController for the given main window. */
	void attach(MainWindow* window) override;
	
	/** Detaches the HomeScreenController from its main window. */
	void detach() override;
	
public slots:
	/** (Re-)reads the settings. */
	void readSettings();
	
	/** Clears the application's list of recently opened files. */
	void clearRecentFiles();
	
	/** Sets whether to open the most recently used file on startup. */
	void setOpenMRUFile(bool state);
	
	/** Sets the visibility of the tip-of-the-day to state. */
	void setTipsVisible(bool state);
	
	/** Moves to the tip following the current tip-of-the-day. */
	void goToPreviousTip();
	
	/** Moves to the tip preceding the current tip-of-the-day. */
	void goToNextTip();
	
	/** Moves to the tip-of-the-day given by index. */
	void goToTip(int index);
	
protected:
	/** The widget owned and controlled by this HomeScreenController. */
	AbstractHomeScreenWidget* widget;
	
	/** The index of the tip-of-the-day currently displayed. */
	int current_tip;
};


}  // namespace LibreMapper

#endif
