/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013, 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "text_browser_dialog.h"

#include <Qt>
#include <QApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QScrollBar>
#include <QScroller>  // IWYU pragma: keep
#include <QString>
#include <QTextBrowser>
#include <QToolTip>
#include <QVBoxLayout>

#include "gui/widgets/text_browser.h"


namespace LibreMapper {

TextBrowserDialog::TextBrowserDialog(QWidget* parent)
: QDialog(parent)
, text_browser(new TextBrowser())
{
	if (parent)
	{
		setWindowModality(Qt::WindowModal);
	}
	
	QVBoxLayout* layout = new QVBoxLayout();
	setLayout(layout);
	
	int left, top, right, bottom;
	layout->getContentsMargins(&left, &top, &right, &bottom);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	
	text_browser->setOpenExternalLinks(true);
	layout->addWidget(text_browser);
	
	QHBoxLayout* buttons_layout = new QHBoxLayout();
	buttons_layout->setContentsMargins(left, top, right, bottom);
	
	QPushButton* back_button  = new QPushButton(QIcon(QStringLiteral(":/images/arrow-left.png")), QApplication::translate("QFileDialog", "Back"));
	back_button->setEnabled(false);
	buttons_layout->addWidget(back_button);
	
	buttons_layout->addStretch(1);
	
	QPushButton* close_button  = new QPushButton(QApplication::translate("QPlatformTheme", "Close"));
	close_button->setDefault(true);
	buttons_layout->addWidget(close_button);
	
	layout->addLayout(buttons_layout);
	
	connect(text_browser, &QTextBrowser::sourceChanged, this, &TextBrowserDialog::sourceChanged);
	connect(text_browser, &QTextBrowser::textChanged, this, &TextBrowserDialog::updateWindowTitle);
	connect(text_browser, &QTextBrowser::backwardAvailable, back_button, &TextBrowserDialog::setEnabled);
	connect(text_browser, QOverload<const QUrl&>::of(&QTextBrowser::highlighted), this, &TextBrowserDialog::highlighted);
	connect(back_button,  &QPushButton::clicked, text_browser, &QTextBrowser::backward);
	connect(close_button, &QPushButton::clicked, this, &TextBrowserDialog::accept);
	
#if defined(Q_OS_ANDROID)
	QScroller::grabGesture(text_browser->viewport(), QScroller::TouchGesture);
	// Disable selection, so that it doesn't interfere with scrolling
	text_browser->setTextInteractionFlags(Qt::TextInteractionFlags(Qt::TextBrowserInteraction) & ~Qt::TextSelectableByMouse);
	// Note: Only the above combination of QScroller::LeftMouseButtonGesture
	// and ~Qt::TextSelectableByMouse seems to achieve the desired behaviour
	// (touch-scrolling without selecting text.)
	
	setWindowState((windowState() & ~(Qt::WindowMinimized | Qt::WindowFullScreen))
                   | Qt::WindowMaximized);
#endif
}


TextBrowserDialog::TextBrowserDialog(const QUrl& initial_url, QWidget* parent)
: TextBrowserDialog(parent)
{
	text_browser->setSource(initial_url);
	text_browser->document()->adjustSize();  // needed for sizeHint()
}


TextBrowserDialog::TextBrowserDialog(const QString& text, QWidget* parent)
 : TextBrowserDialog(parent)
{
	text_browser->setText(text);
	text_browser->document()->adjustSize();  // needed for sizeHint()
}


TextBrowserDialog::~TextBrowserDialog() = default;



QSize TextBrowserDialog::sizeHint() const
{
	QSize size = text_browser->document()->size().toSize();
	if (text_browser->verticalScrollBar())
		size.rwidth() += text_browser->verticalScrollBar()->width();
	return size;
}

void TextBrowserDialog::sourceChanged(const QUrl&)
{
	; // Nothing, to be overridden in subclasses
}

void TextBrowserDialog::updateWindowTitle()
{
	setWindowTitle(text_browser->documentTitle());
}

void TextBrowserDialog::highlighted(const QUrl& link)
{
	if (link.isEmpty())
	{
		QToolTip::hideText();
	}
	else
	{
		/// @todo: Position near mouse pointer
		auto tooltip_pos  = pos() + text_browser->pos();
		tooltip_pos.ry() += text_browser->height();
		QToolTip::showText(tooltip_pos, link.toString(), this, {});
	}
}


}  // namespace LibreMapper
