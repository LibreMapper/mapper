/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013, 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "about_dialog.h"

#include <cmath>

#include <QApplication>
#include <QTextBrowser>

#include "mapper_config.h"


namespace OpenOrienteering {

namespace {

/**
 * @brief An URL identifying the main "about" page.
 * 
 * The main page's text will be set directly, thus not having a valid URL.
 * But an empty URL will be ignored by QTextBrowser's history, leading to
 * unexpected behaviour of backward navigation.
 */
const QUrl& aboutPageUrl()
{
	static auto url = QUrl(QString::fromLatin1("#ABOUT"));
	return url;
}

/**
 * Puts the items of a QStringList into an HTML block or a sequence of blocks.
 */
QString formatBlock(const QStringList& items)
{
#if defined(Q_OS_ANDROID) // or any other small-screen device
	QString block = QLatin1String("<p>")
	                + items.join(QString::fromLatin1(", "))
	                + QLatin1String("</p>");
#else
	QString block;
	block.reserve(100 + 30 * items.size());
	block.append(QLatin1String("<table><tr><td>"));
	constexpr int columns = 3;
	const int rows = (int)ceil((double)items.size() / columns);
	for (int i = 0, row = 1; i < items.size(); ++i)
	{
		block.append(items[i]);
		if (rows != row)
		{
			block.append(QString::fromLatin1("<br/>"));
			++row;
		}
		else if (i < items.size())
		{
			block.append(QString::fromLatin1("</td><td>&nbsp;&nbsp;&nbsp;</td><td>"));
			row = 1;
		}
	}
	block.append(QString::fromLatin1("</td></tr></table>"));
#endif
	return block;
}


}  // namespace



AboutDialog::AboutDialog(QWidget* parent)
 : TextBrowserDialog(aboutPageUrl(), parent)
{
	text_browser->setHtml(about());
	text_browser->document()->adjustSize();
	updateWindowTitle();
}

void AboutDialog::sourceChanged(const QUrl& url)
{
	if (url == aboutPageUrl())
		text_browser->setHtml(about());
}

void AboutDialog::updateWindowTitle()
{
	QString title = text_browser->documentTitle();
	if (title.isEmpty())
		title = tr("About %1").arg(APP_NAME);
	setWindowTitle(title);
}

QString AboutDialog::about()
{
	const QStringList maintainers_list( QStringList()
	  << QString::fromUtf8("<b>Libor Pecháček</b>")
	);

	const QStringList oo_developers_list( QStringList()
	  << QString::fromLatin1("Peter Curtis (2012-2013)")
	  << QString::fromLatin1("Kai Pastor")
	  << QString::fromUtf8("Libor Pecháček (2017-2024)")
	  << QString::fromUtf8("Thomas Schöps (2012-2014, %1)")
	);
	
	const QStringList contributors_list( QStringList()
	  << QString::fromLatin1("Arrizal Amin")
	  << QString::fromLatin1("Javier Arufe")
	  << QString::fromLatin1("Jonathan Bakker")
	  << QString::fromLatin1("Eric Boulet")
	  << QString::fromLatin1("Jon Cundill")
	  << QString::fromUtf8("Sławomir Cygler")
	  << QString::fromLatin1("Jan Dalheimer")
	  << QString::fromLatin1("Davide De Nardis")
	  << QString::fromLatin1("Eugeniy Fedirets")
	  << QString::fromLatin1("Joao Franco")
	  << QString::fromLatin1("Pavel Fric")
	  << QString::fromLatin1("Naofumi Fukue")
	  << QString::fromLatin1("Anders Gressli")
	  << QString::fromLatin1("Peter Hoban")
	  << QString::fromLatin1("Henrik Johansson")
	  << QString::fromLatin1("Panu Karhu")
	  << QString::fromLatin1("Oskar Karlin")
	  << QString::fromLatin1("Nikolay Korotkiy")
	  << QString::fromLatin1("Mitchell Krome")
	  << QString::fromUtf8("Matthias Kühlewein")
	  << QString::fromLatin1("Albin Larsson")
	  << QString::fromUtf8("István Marczis")
	  << QString::fromLatin1("Tojo Masaya")
	  << QString::fromLatin1("Yevhen Mazur")
	  << QString::fromLatin1("Fraser Mills")
	  << QString::fromLatin1("Vincent Poinsignon")
	  << QString::fromLatin1("Russell Porter")
	  << QString::fromLatin1("Adhika Setya Pramudita")
	  << QString::fromLatin1("Christopher Schive")
	  << QString::fromLatin1("Arif Suryawan")
	  << QString::fromLatin1("Jan-Gerard van der Toorn")
	  << QString::fromLatin1("Scott Turner")
	  << QString::fromLatin1("Semyon Yakimov")
	  << QString::fromLatin1("Aivars Zogla")
	);
	
	QString mapper_about = QString::fromLatin1(
	  "<html><head>"
	  "<title>%1</title>"
	  "</head><body>"
	  "<table><tr>"
	  "<td><img src=\":/images/mapper-icon/Mapper-128.png\"/></td>"
	  "<td><h1>%2</h1><em>%5</em>%6</td>"
	  "</tr></table>"
	  "<p>Copyright %3<br/>%4</p>"
	  "<p>%7</p>"
	  "<p>%8</p>"
	  "<p>%9</p>"
	  "<p>%10%11</p>"
	  "<p>&nbsp;<hr/>%12%13</p>"
	  "<p>%14%15</p>"
	  "</body></html>"
	).arg(
	  tr("About %1").arg(APP_NAME), // %1
	  qApp->applicationDisplayName(),   // %2
	  QString::fromLatin1(APP_COPYRIGHT),   // %3
	  tr("LibreMapper is based on OpenOrienteering Mapper."), // %4
	  tr("A free software for drawing orienteering maps"), // %5
	  QLatin1Literal(""),   // %6, placeholder for the website
	  tr("This program is free software: you can redistribute it "
	     "and/or modify it under the terms of the "
	     "<a %1>GNU General Public License (GPL), version&nbsp;3</a>, "
	     "as published by the Free Software Foundation.").arg(QString::fromLatin1("href=\"file:doc:/common-licenses/GPL-3.txt\"")), // %7
	  tr("This program is distributed in the hope that it will be useful, "
	     "but WITHOUT ANY WARRANTY; without even the implied warranty of "
	     "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
	     "GNU General Public License (GPL), version&nbsp;3, for "
	     "<a %1>more details</a>.").arg(QString::fromLatin1("href=\"file:doc:/common-licenses/GPL-3.txt#L589\"")), // %8
	  tr("<a %1>All about licenses, copyright notices, conditions and disclaimers.</a>").arg(QString::fromLatin1("href=\"file:doc:/licensing.html\"")) // %9
	).arg(
	  tr("LibreMapper maintainers:"), // %10
	  formatBlock(maintainers_list), // %11
	  tr("OpenOrienteering developers in alphabetical order:"), // %12
	  formatBlock(oo_developers_list).arg(tr("(project initiator)").replace(QLatin1Char('('), QString{}).replace(QLatin1Char(')'), QString{})), // %13
	  tr("OpenOrienteering contributors:"), // %14
	  formatBlock(contributors_list) // %15
	);
	
	return mapper_about;
}


}  // namespace OpenOrienteering
