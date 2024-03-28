/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Jan Dalheimer (OpenOrienteering)
 * Copyright 2013-2016  Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_SETTINGS_DIALOG_H
#define OPENORIENTEERING_SETTINGS_DIALOG_H

#include <QDialog>
#include <QObject>
#include <QString>

class QAbstractButton;
class QCloseEvent;
class QDialogButtonBox;
class QKeyEvent;
class QResizeEvent;
class QScrollArea;
class QStackedWidget;
class QTabWidget;
class QWidget;

namespace OpenOrienteering {

class SettingsPage;


/** 
 * A dialog for editing Mapper's settings.
 * 
 * This class is marked as final because its constructor calls virtual functions.
 */
class SettingsDialog final : public QDialog
{
Q_OBJECT
public:
	/** 
	 * Constructs a new settings dialog.
	 */
	explicit SettingsDialog(QWidget* parent = nullptr);
	
	/** 
	 * Destroys the settings dialog.
	 */
	~SettingsDialog() override;
	
protected:
	/**
	 * Adds all known pages to the dialog.
	 * 
	 * This function is called from the constructor. It may be overridden to
	 * provide dialogs with different pages.
	 */
	virtual void addPages();
	
	/**
	 * Adds a single page to the dialog.
	 */
	void addPage(SettingsPage* page);
	
	/**
	 * Calls a SettingsPage member function on all pages.
	 */
	void callOnAllPages(void (SettingsPage::*member)());
	
	
	void closeEvent(QCloseEvent* event) override;
	
	void keyPressEvent(QKeyEvent* event) override;
	
	void resizeEvent(QResizeEvent* event) override;
	
	void resizeToFit(QScrollArea& widget);
	
private slots:
	/**
	 * Reacts to dialog buttons (OK, Cancel, Rest).
	 */
	void buttonPressed(QAbstractButton* button);
	
private:
	/**
	 * A tab widget which holds all pages in desktop mode.
	 */
	QTabWidget* tab_widget;
	
	/**
	 * A stack widget which holds all pages in mobile mode.
	 */
	QStackedWidget* stack_widget;
	
	/** 
	 * The box of standard dialog buttons.
	 */
	QDialogButtonBox* button_box;
	
	int scrollbar_extent;
};


}  // namespace OpenOrienteering

#endif
