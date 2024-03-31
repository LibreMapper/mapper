/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "symbol_tooltip.h"

#include <Qt>
#include <QtGlobal>
#include <QApplication>
#include <QCursor>
#include <QDesktopWidget>
#include <QHideEvent>
#include <QLabel>
#include <QLatin1Char>
#include <QLatin1String>
#include <QPainter>
#include <QPalette>
#include <QPoint>
#include <QShortcut>
#include <QShowEvent>
#include <QSize>
#include <QString>
#include <QStyle>
#include <QStyleOption>
#include <QVBoxLayout>

#include "core/map.h"
#include "core/symbols/symbol.h"

// IWYU pragma: no_forward_declare QWidget


namespace LibreMapper {

SymbolToolTip::SymbolToolTip(QWidget* parent, QShortcut* shortcut)
: QWidget{ parent }
, shortcut{ shortcut }
, symbol{ nullptr }
, description_shown{ false }
{
	setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_OpaquePaintEvent);
	
	QPalette text_palette;
	text_palette.setColor(QPalette::Window, text_palette.color(QPalette::Base));
	text_palette.setColor(QPalette::WindowText, text_palette.color(QPalette::Text));
	setPalette(text_palette);
	
	name_label = new QLabel();
	description_label = new QLabel();
	description_label->setWordWrap(true);
	description_label->hide();
	
	auto layout = new QVBoxLayout();
	QStyleOption style_option;
	layout->setContentsMargins(
	  style()->pixelMetric(QStyle::PM_LayoutLeftMargin, &style_option) / 2,
	  style()->pixelMetric(QStyle::PM_LayoutTopMargin, &style_option) / 2,
	  style()->pixelMetric(QStyle::PM_LayoutRightMargin, &style_option) / 2,
	  style()->pixelMetric(QStyle::PM_LayoutBottomMargin, &style_option) / 2
	);
	layout->addWidget(name_label);
	layout->addWidget(description_label);
	setLayout(layout);
	
	tooltip_timer.setSingleShot(true);
	connect(&tooltip_timer, &QTimer::timeout, this, &QWidget::show);
	
	if (shortcut)
	{
		shortcut->setEnabled(false);
		connect(shortcut, &QShortcut::activated, this, &SymbolToolTip::showDescription);
	}
}

void SymbolToolTip::showDescription()
{
	if (symbol && !description_shown && isVisible())
	{
		description_label->show();
		adjustSize();
		adjustPosition(false);
		description_shown = true;
	}
	
	if (shortcut)
		shortcut->setEnabled(false);
}

void SymbolToolTip::reset()
{
	symbol = nullptr;
	tooltip_timer.stop();
	hide();
	description_label->hide();
	description_shown = false;
	
	if (shortcut)
		shortcut->setEnabled(false);
}

void SymbolToolTip::enterEvent(QEvent* event)
{
	Q_UNUSED(event);
    hide();
}

void SymbolToolTip::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	
	QPainter painter(this);
	
	painter.setBrush(palette().color(QPalette::Window));
	painter.setPen(palette().color(QPalette::WindowText));
	
	QRect rect(0, 0, width() - 1, height() - 1);
	painter.drawRect(rect);
	
	painter.end();
}

void SymbolToolTip::adjustPosition(bool mobile_mode)
{
	auto size = this->size();
	auto desktop = QApplication::desktop()->screenGeometry(QCursor::pos());
	
	const int margin = 3;
	bool has_room_to_left  = (icon_rect.left()   - size.width()  - margin >= desktop.left());
	bool has_room_to_right = (icon_rect.right()  + size.width()  + margin <= desktop.right());
	bool has_room_above    = (icon_rect.top()    - size.height() - margin >= desktop.top());
	bool has_room_below    = (icon_rect.bottom() + size.height() + margin <= desktop.bottom());
	if (!has_room_above && !has_room_below && !has_room_to_left && !has_room_to_right) {
		return;
	}
	
	if (mobile_mode)
	{
		// Change precedence.
		if (has_room_above)
		{
			has_room_below = false;
		}
		else if (has_room_to_left)
		{
			has_room_below = false;
			has_room_to_right = false;
		}
		else if (has_room_to_right)
		{
			has_room_below = false;
		}
	}
		
	int x = 0;
	int y = 0;
	
	if (has_room_below || has_room_above)
	{
		y = has_room_below ? icon_rect.bottom() + margin : icon_rect.top() - size.height() - margin;
		x = qMin(qMax(desktop.left() + margin, icon_rect.center().x() - size.width() / 2), desktop.right() - size.width() - margin);
	} else {
		x = has_room_to_right ? icon_rect.right() + margin : icon_rect.left() - size.width() - margin;
		y = qMin(qMax(desktop.top() + margin, icon_rect.center().y() - size.height() / 2), desktop.bottom() - size.height() - margin);
	}
	
	move(QPoint(x, y));
}

void SymbolToolTip::scheduleShow(const Symbol* symbol, const Map* map, QRect icon_rect, bool mobile_mode)
{
	this->icon_rect = icon_rect;
	this->symbol = symbol;
	
	Q_ASSERT(map);
	name_label->setText(symbol->getNumberAsString()
	                    + QLatin1String(" <b>")
	                    + map->translate(symbol->getName())
	                    + QLatin1String("</b>"));
	
	auto help_text = map->translate(symbol->getDescription());
	if (help_text.isEmpty())
	{
		help_text = tr("No description!");
	}
	else
	{
		help_text.replace(QLatin1Char('\n'), QStringLiteral("<br>"));
		help_text.remove(QLatin1Char('\r'));
	}
	description_label->setText(help_text);
	description_label->hide();
	description_shown = false;
	shortcut->setEnabled(isVisible());
	
	adjustSize();
	adjustPosition(mobile_mode);
	
	static const int delay = 150;
	tooltip_timer.start(delay);
}

void SymbolToolTip::showEvent(QShowEvent* event)
{
	if (shortcut && !description_shown && !event->spontaneous())
		shortcut->setEnabled(true);
	
	QWidget::showEvent(event);
}

void SymbolToolTip::hideEvent(QHideEvent* event)
{
	if (!event->spontaneous())
		reset();
	
	QWidget::hideEvent(event);
}

const Symbol* SymbolToolTip::getSymbol() const
{
	return symbol;
}


}  // namespace LibreMapper
