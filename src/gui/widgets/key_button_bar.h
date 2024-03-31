/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_KEY_BUTTON_BAR_H
#define LIBREMAPPER_KEY_BUTTON_BAR_H

#include <Qt>
#include <QtGlobal>
#include <QIcon>
#include <QObject>
#include <QString>
#include <QVarLengthArray>
#include <QWidget>

class QEvent;
class QHBoxLayout;
class QHideEvent;
class QShowEvent;
class QToolButton;

namespace OpenOrienteering {


/**
 * Shows a set of buttons for simulating keyboard input.
 * 
 * This is used to simulate keys in Mapper's mobile GUI where a physical
 * keyboard is not present.
 * 
 * For modifier keys, the GUI buttons indicate whether the keys are in pressed
 * state or not. However, the labels are not expected to display the name of the
 * key but the associated tool behaviour. The tools need to make sure that the
 * actual behaviour is consistent with the indicated state.
 */
class KeyButtonBar : public QWidget
{
Q_OBJECT
public:
	/**
	 * Constructs a new button bar acting on the given receiver.
	 * 
	 * The constructor will install this object as an event filter on the given
	 * receiver, independent of object ownership.
	 */
	KeyButtonBar(QWidget* receiver, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	
	~KeyButtonBar() override;
	
	
	/**
	 * Adds a button for a regular key.
	 */
	QToolButton* addKeyButton(Qt::Key key_code, const QString& text, const QIcon& icon = {});
	
	/**
	 * Adds a button for a combination of a regular key with a modifier key.
	 * 
	 * By default, the button is not checkable and sends a sequence of a press
	 * event and a release event for the given key_code. If a modifier_code
	 * different from Qt::NoModifier is given, and this modifier key is not
	 * already active, a press event for the corresponding modifier key code is
	 * sent first, and a release event for the modifier key code is send after
	 * the the event for the key_code.
	 * 
	 * Application code may turn this into a checkable button if it also takes
	 * care of maintaining the state.
	 */
	QToolButton* addKeyButton(Qt::Key key_code, Qt::KeyboardModifier modifier_code, const QString& text, const QIcon& icon = {});
	
	
	/** Adds a checkable button for a modifier key.
	 * 
	 * The button sends a key press event when the user activates the checked
	 * state, and it sends a key release event when the user unsets the checked
	 * state.
	 * 
	 * Upon regular input events for the corresponding modifier key, the checked
	 * state of the button is updated accordingly.
	 */
	QToolButton* addModifierButton(Qt::KeyboardModifier modifier_code, const QString& text, const QIcon& icon = {});
	
	
	/**
	 * Returns the currently active set of modifier keys.
	 */
	Qt::KeyboardModifiers keyboardModifiers() const { return active_modifiers; }
	
	
	/**
	 * The event filter synchronizes the buttons' state and changes with input events.
	 * 
	 * It acts on the receiver widget given in the KeyButtonBar constructor.
	 */
	bool eventFilter(QObject* watched, QEvent* event) override;
	
	
protected:
	void showEvent(QShowEvent* event) override;
	
	void hideEvent(QHideEvent* event) override;
	
	
private:
	void sendKeyPressAndRelease();
	
	void sendModifierChange(bool checked);
	
	void sendKeyPressEvent(Qt::Key key_code);
	
	void sendKeyReleaseEvent(Qt::Key key_code);
	
	
	struct ButtonInfo
	{
		QToolButton*         button;
		Qt::Key              key;
		Qt::KeyboardModifier modifier;
	};
	
	QVarLengthArray<ButtonInfo, 10> button_infos;
	QWidget* receiver;
	QHBoxLayout* layout;
	Qt::KeyboardModifiers active_modifiers;
	bool active = false;
	
	
	Q_DISABLE_COPY(KeyButtonBar)
};


}  // namespace OpenOrienteering

#endif // LIBREMAPPER_KEY_BUTTON_BAR_H
