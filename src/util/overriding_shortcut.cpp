/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "overriding_shortcut.h"

#include <QEvent>
#include <QKeyEvent>
#include <QShortcutEvent>
#include <QWidget>


namespace LibreMapper {

// ### OverridingShortcut ###

OverridingShortcut::OverridingShortcut(QWidget* parent)
 : OverridingShortcut(QKeySequence{}, parent, nullptr, nullptr, Qt::WindowShortcut)
{}

OverridingShortcut::OverridingShortcut(const QKeySequence& key, QWidget* parent, const char* member, const char* ambiguousMember, Qt::ShortcutContext context)
 : QShortcut(key, parent, member, ambiguousMember, context)
 , parent_or_self(parent)
{
	if (!parent_or_self)
		parent_or_self = this;
	parent_or_self->installEventFilter(this);
	updateEventFilters();
}


OverridingShortcut::~OverridingShortcut()
{
	parent_or_self->removeEventFilter(this);
	if (window_)
		window_->removeEventFilter(this);
}



bool OverridingShortcut::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::ParentChange
	    && (watched == parent_or_self || watched == window_) )
	{
		updateEventFilters();
	}
	else if (event->type() == QEvent::ShortcutOverride
	         && isEnabled() && key().count() == 1)
	{
		auto* key_event = static_cast<QKeyEvent*>(event);
		if ((key_event->key() | int(key_event->modifiers())) == key()[0])
		{
			QShortcutEvent se(key(), id());
			event->setAccepted(QShortcut::event(&se));
			return event->isAccepted();
		}
	}
	
	return false;
}


void OverridingShortcut::updateEventFilters()
{
	QObject* new_parent_or_self = parent() ? parent() : this; 
	if (parent_or_self != new_parent_or_self)
	{
		parent_or_self->removeEventFilter(this);
		parent_or_self = new_parent_or_self;
		parent_or_self->installEventFilter(this);
	}
	
	QWidget* parent_widget = qobject_cast<QWidget*>(parent());
	QWidget* new_window = parent_widget ? parent_widget->window() : nullptr;
	if (window_ != new_window)
	{
		if (window_)
			window_->removeEventFilter(this);
		window_ = new_window;
		if (window_)
			window_->installEventFilter(this);
	}
}


}  // namespace LibreMapper
