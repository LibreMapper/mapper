/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_OVERRIDING_SHORTCUT_H
#define LIBREMAPPER_OVERRIDING_SHORTCUT_H

#include <Qt>
#include <QKeySequence>
#include <QObject>
#include <QPointer>
#include <QShortcut>
#include <QString>
#include <QWidget>

class QEvent;

namespace LibreMapper {


/**
 * OverridingShortcut is a variation of QShortcut which takes precedence over
 * other listeners for the same key sequence.
 * 
 * It does so by reacting on events of type of QEvent::ShortcutOverride. Since
 * these events are of class QKeyEvent, the overriding works only for key
 * sequences consisting of a single key plus modifiers. For multi-key
 * sequences, the shortcut will work like a normal QShortcut.
 */
class OverridingShortcut : public QShortcut
{
Q_OBJECT
public:
	/**
	 * Constructs a OverridingShortcut object for the given parent widget.
	 * parent must not be nullptr.
	 * @see QShortcut::QShortcut(QWidget*)
	 */
	OverridingShortcut(QWidget* parent);
	
	/**
	 * Constructs a OverridingShortcut object.
	 * parent must not be nullptr.
	 * @see QShortcut::QShortcut(const QKeySequence&, QWidget*, const char*, const char*, Qt::ShortcutContext)
	 */
	OverridingShortcut(const QKeySequence& key, QWidget* parent, const char* member = nullptr, const char* ambiguousMember = nullptr, Qt::ShortcutContext context = Qt::WindowShortcut);
	
	OverridingShortcut(const OverridingShortcut&) = delete;
	OverridingShortcut(OverridingShortcut&&) = delete;
	OverridingShortcut& operator=(const OverridingShortcut&) = delete;
	OverridingShortcut& operator=(OverridingShortcut&&) = delete;
	
	~OverridingShortcut() override;
	
	/**
	 * Filters events of type QEvent::ShortcutOverride which match this
	 * shortcut's key sequence, and passes them as corresponding
	 * QShortcutEvent to QShortcut::event().
	 */
	bool eventFilter(QObject* watched, QEvent* event) override;
	
private:
	void updateEventFilters();
	
	QObject* parent_or_self;
	QPointer<QWidget> window_ = nullptr;
};


}  // namespace LibreMapper

#endif
