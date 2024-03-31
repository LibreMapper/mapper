/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013, 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "modifier_key.h"

#include <QFlags>
#include <QLatin1Char>
#include <QKeySequence>


namespace LibreMapper {

ModifierKey::ModifierKey(int key)
 : native_text { QKeySequence((int)key).toString(QKeySequence::NativeText) }
{
	if (native_text.endsWith(QLatin1Char('+')))
	{
		native_text.chop(1);
	}
}

ModifierKey::ModifierKey(Qt::KeyboardModifiers keys)
 : ModifierKey { (int)keys }
{
	// nothing else
}

ModifierKey::ModifierKey(Qt::Key key)
 : ModifierKey { (int)key }
{
	// nothing else
}

const ModifierKey& ModifierKey::alt()
{
	static const ModifierKey key(Qt::AltModifier);
	return key;
}

const ModifierKey& ModifierKey::control()
{
	static const ModifierKey key(Qt::ControlModifier);
	return key;
}

const ModifierKey& ModifierKey::controlShift()
{
	static const ModifierKey key(Qt::ControlModifier | Qt::ShiftModifier);
	return key;
}

const ModifierKey& ModifierKey::meta()
{
	static const ModifierKey key(Qt::MetaModifier);
	return key;
}

const ModifierKey& ModifierKey::shift()
{
	static const ModifierKey key(Qt::ShiftModifier);
	return key;
}

const ModifierKey& ModifierKey::space()
{
	static const ModifierKey key(Qt::Key_Space);
	return key;
}

const ModifierKey& ModifierKey::return_key()
{
	static const ModifierKey key(Qt::Key_Return);
	return key;
}

const ModifierKey& ModifierKey::backspace()
{
	static const ModifierKey key(Qt::Key_Backspace);
	return key;
}

const ModifierKey& ModifierKey::escape()
{
	static const ModifierKey key(Qt::Key_Escape);
	return key;
}


}  // namespace LibreMapper
