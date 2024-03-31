/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013, 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_ABOUT_DIALOG_H
#define LIBREMAPPER_ABOUT_DIALOG_H

#include <QObject>
#include <QString>

class QUrl;
class QWidget;

#include "gui/text_browser_dialog.h"

namespace OpenOrienteering {


/**
 * @brief A dialog which shows information about Mapper and its components.
 */
class AboutDialog : public TextBrowserDialog
{
Q_OBJECT
public:
	/**
	 * @brief Construct a new AboutDialog.
	 */
	AboutDialog(QWidget* parent = nullptr);
	
	/**
	 * @brief Returns the basic information about this software.
	 * The return string may contain HTML formatting.
	 */
	static QString about();
	
protected:
	/**
	 * @brief Sets custom HTML content when the URL identifies the first page.
	 */
	void sourceChanged(const QUrl& url) override;
	
	/**
	 * @brief Updates the window title from the current document title.
	 */
	void updateWindowTitle() override;
};


}  // namespace OpenOrienteering

#endif
