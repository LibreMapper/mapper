/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014 Thomas Schöps, Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_MAIN_WINDOW_CONTROLLER_H
#define LIBREMAPPER_MAIN_WINDOW_CONTROLLER_H

#include <QObject>
#include <QString>

class QKeyEvent;
class QWidget;

namespace OpenOrienteering {

class MainWindow;
class FileFormat;


/** A MainWindowController provides the specific content and 
 *  behaviours for a main window, for example map drawing or
 *  course setting functions.
 */
class MainWindowController : public QObject
{
Q_OBJECT
public:
	
	~MainWindowController() override;
	
	
	/**
	 * Returns true when the controller wants a menu bar to be shown.
	 * 
	 * The default implementation returns false for mobile mode,
	 * true otherwise.
	 */
	virtual bool menuBarVisible();
	
	/**
	 * Returns true when the controller wants a status bar to be shown.
	 * 
	 * The default implementation returns false for mobile mode,
	 * true otherwise.
	 */
	virtual bool statusBarVisible();
	
	
	/** Save to a file.
	 *  @param path the path to save to
	 *  @param format the file format
	 *  @return true if saving was successful, false on errors
	 */
	virtual bool saveTo(const QString& path, const FileFormat& format);
	
	/** Export to a file, but don't change modified state
	 *  with regard to the original file.
	 *  @param path the path to export to
	 *  @return true if saving was successful, false on errors
	 */
	bool exportTo(const QString& path);

	/** Export to a file, but don't change modified state
	 *  with regard to the original file.
	 *  @param path the path to export to
	 *  @param format the file format
	 *  @return true if saving was successful, false on errors
	 */
	virtual bool exportTo(const QString& path, const FileFormat& format);

	/** Load from a file.
	 *  @param path the path to load from
	 *  @param dialog_parent Alternative parent widget for all dialogs.
	 *      If nullptr, implementations should use MainWindowController::window.
	 *  @return true if loading was successful, false on errors
	 */
	virtual bool loadFrom(const QString& path, const FileFormat& format, QWidget* dialog_parent = nullptr);
	
	/** Attach the controller to a main window. 
	 *  The controller should create its user interface here.
	 */
	virtual void attach(MainWindow* window) = 0;
	
	/** Detach the controller from a main window. 
	 *  The controller should delete its user interface here.
	 */
	virtual void detach();
	
	/**
	 * Returns true when editing is in progress.
	 * 
	 * "Editing in progress" means the file is an "unstable" state where no
	 * global operations like save, undo, redo shall not be applied.
	 */
	virtual bool isEditingInProgress() const;
	
	/**
	 * @brief Receives key press events from the main window.
	 * 
	 * QKeyEvent starts with isAccepted() == true, so the return value of this
	 * function decides if the event shall be stopped from being handled further.
	 * 
	 * The default implementation simply returns false.
	 * 
	 * @return True if the event shall be stopped from being handled further, false otherwise.
	 */
	virtual bool keyPressEventFilter(QKeyEvent* event);
	
	/**
	 * @brief Receives key release events from the main window.
	 * 
	 * QKeyEvent starts with isAccepted() == true, so the return value of this
	 * function decides if the event shall be stopped from being handled further.
	 * 
	 * The default implementation simply returns false.
	 * 
	 * @return True if the event shall be stopped from being handled further, false otherwise.
	 */
	virtual bool keyReleaseEventFilter(QKeyEvent* event);
	
	/** Get the main window this controller is attached to.
	 */
	inline MainWindow* getWindow() const;
	
	/** Get a controller suitable for a particular file.
	 *  @param filename the name of the file
	 *  @return a MainWindowController that is able to load the file
	 */
	static MainWindowController* controllerForFile(const QString& filename);
	
protected:
	MainWindow* window;
};



//### MainWindowController inline code ###

inline
MainWindow* MainWindowController::getWindow() const
{
	return window;
}


}  // namespace OpenOrienteering

#endif
