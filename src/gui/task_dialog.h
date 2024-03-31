/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_UTIL_TASK_DIALOG_H
#define LIBREMAPPER_UTIL_TASK_DIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QObject>
#include <QString>

class QAbstractButton;
class QCommandLinkButton;
class QPushButton;
class QSignalMapper;
class QVBoxLayout;
class QWidget;

namespace LibreMapper {


/**
 * Shows a dialog similar to a message box, but with the option to add
 * one or multiple command buttons to be able to offer more and well-described
 * actions to the user.
 * 
 * Using this class might look like this:
 *     TaskDialog dialog(parent, "Title", "Text", QDialogButtonBox::Abort);
 *     QAbstractButton* do_button = dialog.addCommandButton("Do it", "description ...");
 *     QAbstractButton* dont_button = dialog.addCommandButton("Don't do it", "description ...");
 *     dialog.exec();
 *     if (dialog.clicked_button() == do_button)
 *         ...
 *     else if (dialog.clicked_button() == dont_button)
 *         ...
 *     else // if (dialog.clicked_button() == dialog.button(QDialogButtonBox::Abort))
 *         ...
 */
class TaskDialog : public QDialog
{
Q_OBJECT
public:
	/**
	 * Constructs a TaskDialog.
	 * 
	 * @param parent QWidget parent for the dialog.
	 * @param title Dialog title.
	 * @param text Text which will be displayed in a label at the top.
	 * @param buttons Dialog standard buttons.
	 */
	TaskDialog(
		QWidget* parent,
		const QString& title,
		const QString& text,
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton
	);
	
	/**
	 * Adds a command button to the dialog. Returns a pointer to the created button.
	 * 
	 * @param text The action text, will be emphasized
	 * @param description Additional description text for the action, optional
	 */
	QCommandLinkButton* addCommandButton(
		const QString& text,
		const QString& description
	);
	
	
	/** After the dialog has been shown, returns the button which
	 *  was clicked by the user. */
	inline QAbstractButton* clickedButton() const
	{
		return clicked_button;
	}
	
	/** Returns a button pointer for a specific standard dialog button.
	 *  Use this for comparing the pointer with the result of clickedButton(). */
	inline QPushButton* button(QDialogButtonBox::StandardButton which) const
	{
		return button_box->button(which);
	}
	
private slots:
	void buttonClicked(QWidget* button);
	void buttonClicked(QAbstractButton* button);
	
private:
	QAbstractButton* clicked_button;
	QVBoxLayout* layout;
	QDialogButtonBox* button_box;
	QSignalMapper* signal_mapper;
};


}  // namespace LibreMapper

#endif
