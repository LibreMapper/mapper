/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "text_browser.h"


namespace LibreMapper {

TextBrowser::TextBrowser(QWidget* parent)
: QTextBrowser{ parent }
{
	// nothing else
}


TextBrowser::~TextBrowser() = default;



QVariant TextBrowser::loadResource(int type, const QUrl& name)
{
	auto result = QTextBrowser::loadResource(type, name);
	if (result.typeId() == QVariant::ByteArray
	    && type == QTextDocument::HtmlResource
	    && !name.fileName().contains(QLatin1String(".htm"), Qt::CaseInsensitive))
	{
		auto html = QString {
		  QLatin1String("<html><head><title>") +
		    name.fileName().toHtmlEscaped() +
		  QLatin1String("</title></head><body><pre>") +
		    QString::fromUtf8(result.toByteArray()).toHtmlEscaped() +
		  QLatin1String("</pre></body></html>")
		};
		result = QVariant{ html };
	}
	return result;
}


}  // namespace LibreMapper
