/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_UTIL_FILE_DIALOG_H
#define OPENORIENTEERING_UTIL_FILE_DIALOG_H

#include <QFileDialog>
#include <QString>

class QWidget;


namespace OpenOrienteering {

/**
 * A collection of file dialog utility functions.
 */
namespace FileDialog {
	
	/**
	 * Returns true if upper case extensions have to be added explicitly
	 * to filters in file dialogs.
	 */
	bool needUpperCaseExtensions();
	
	
	/**
	 * Adjusts filter and options for file dialogs.
	 * 
	 * Adds upper case version of the extension when needed.
	 * Sets QFileDialog::HideNameFilterDetails when the length of any particular
	 * filter exceeds a certain threshold.
	 */
	void adjustParameters(QString& filter, QFileDialog::Options& options);
	
	
	/**
	 * Calls QFileDialog::getOpenFileName with adjusted parameters.
	 * 
	 * \see adjustParameters, QFileDialog::getOpenFileName
	 */
	inline
	QString getOpenFileName(QWidget* parent = nullptr,
	                        const QString& caption = {},
	                        const QString& dir = {},
	                        QString filter = {},
	                        QString* selected_filter = nullptr,
	                        QFileDialog::Options options = {})
	{
		adjustParameters(filter, options);
		return QFileDialog::getOpenFileName(parent, caption, dir, filter, selected_filter, options);
	}
	
	
	/**
	 * Calls QFileDialog::getSaveFileName with adjusted parameters.
	 * 
	 * \see adjustParameters, QFileDialog::getSaveFileName
	 */
	inline
	QString getSaveFileName(QWidget* parent = nullptr,
	                        const QString& caption = {},
	                        const QString& dir = {},
	                        QString filter = {},
	                        QString* selected_filter = nullptr,
	                        QFileDialog::Options options = {})
	{
		adjustParameters(filter, options);
		return QFileDialog::getSaveFileName(parent, caption, dir, filter, selected_filter, options);
	}
	
	
}  // namespace FileDialog


}  // namespace OpenOrienteering

#endif
