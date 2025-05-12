/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013, 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2015, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_TEXT_BROWSER_DIALOG_H
#define LIBREMAPPER_TEXT_BROWSER_DIALOG_H

#include <QDialog>
#include <QObject>
#include <QSize>
#include <QString>

class QWidget;

class QTextBrowser;
class QUrl;


namespace LibreMapper {

/**
 * @brief A dialog for basic browsing of HTML pages.
 */
class TextBrowserDialog : public QDialog
{
Q_OBJECT
protected:
	explicit TextBrowserDialog(QWidget* parent = nullptr);
	
public:
	/**
	 * @brief Construct a new dialog and loads the text from the initial_url.
	 */
	TextBrowserDialog(const QUrl& initial_url, QWidget* parent = nullptr);
	
	/**
	 * @brief Construct a new dialog and sets the given text.
	 */
	TextBrowserDialog(const QString& text, QWidget* parent = nullptr);
	
	~TextBrowserDialog() override;
	
protected slots:
	/**
	 * @brief Sets custom HTML content when the URL identifies the first page.
	 */
	virtual void sourceChanged(const QUrl& url);
	
	/**
	 * @brief Updates the window title from the current document title.
	 */
	virtual void updateWindowTitle();
	
	/**
	 * @brief Displays a tooltip showing the link if it's an external document.
	 */
	void highlighted(const QUrl& link);
	
protected:
	/**
	 * @brief Returns a size hint based on the text browser's content.
	 */
	QSize sizeHint() const override;
	
	/**
	 * @brief The TextBrowser is the main widget of this dialog.
	 */
	QTextBrowser* const text_browser;
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_TEXT_BROWSER_DIALOG_H
