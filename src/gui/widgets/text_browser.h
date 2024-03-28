/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_TEXT_BROWSER_H
#define OPENORIENTEERING_TEXT_BROWSER_H

#include <QObject>
#include <QTextBrowser>
#include <QVariant>

class QUrl;
class QWidget;

namespace OpenOrienteering {


/**
 * A text browser which handles .txt files.
 * 
 * This class reimplements the loadResource() function so that .txt resources
 * are turned into preformatted HTML.
 */
class TextBrowser: public QTextBrowser
{
Q_OBJECT
public:
	TextBrowser(QWidget* parent = nullptr);
	
	~TextBrowser() override;
	
	QVariant loadResource(int type, const QUrl& name) override;
};


}  // namespace OpenOrienteering

#endif
