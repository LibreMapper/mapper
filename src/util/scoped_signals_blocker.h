/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_UTIL_SCOPED_SIGNALS_BLOCKER_H
#define LIBREMAPPER_UTIL_SCOPED_SIGNALS_BLOCKER_H

#include <QVarLengthArray>

class QObject;

namespace OpenOrienteering {


/**
 * @brief A safe and scoped wrapper around QObject::blockSignals() of multiple objects.
 * 
 * A ScopedMultiSignalsBlocker allows to disable the signals of multiple
 * QObjects for the scope of a particular block. 
 * It sets the QObject's signalsBlocked property to true when they are added
 * (by operator <<), and resets its to its previous state during destruction.
 * 
 * Synopsis:
 * 
 *     {
 *         ScopedMultiSignalsBlocker block(my_widget, my_other_widget);
 *         my_widget->setSomeProperty("Hello World");
 *     }
 * 
 *     {
 *         ScopedMultiSignalsBlocker block;
 *         block << my_widget << my_other_widget;
 *         my_widget->setSomeProperty("Hello World");
 *     }
 */
class ScopedMultiSignalsBlocker
{
private:
	struct item_type
	{
		QObject* object;
		bool old_state;
	};

public:
	ScopedMultiSignalsBlocker() = default;
	
	template <typename ... QObjectPointers>
	ScopedMultiSignalsBlocker(QObjectPointers ... objects);
		          
	~ScopedMultiSignalsBlocker();
	
	void add(QObject* object);
	
	ScopedMultiSignalsBlocker& operator<<(QObject* object);
	
private:
	ScopedMultiSignalsBlocker(const ScopedMultiSignalsBlocker&) = delete;
	ScopedMultiSignalsBlocker& operator=(const ScopedMultiSignalsBlocker&) = delete;
	
	template <typename  ... QObjectPointers>
	void add(QObject* object, QObjectPointers ... others);
	
	void add() const noexcept;
	
	QVarLengthArray<item_type, 10> items;
};


// ### ScopedMultiSignalsBlocker inline code ###

template <typename  ... QObjectPointers>
ScopedMultiSignalsBlocker::ScopedMultiSignalsBlocker(QObjectPointers ... objects)
{
	add(objects ...);
}

template <typename  ... QObjectPointers>
void ScopedMultiSignalsBlocker::add(QObject* object, QObjectPointers ... others)
{
	add(object);
	add(others ...);
}

inline
ScopedMultiSignalsBlocker& ScopedMultiSignalsBlocker::operator<<(QObject* object)
{
	add(object);
	return *this;
}

inline
void ScopedMultiSignalsBlocker::add() const noexcept
{
	// nothing
}


}  // namespace OpenOrienteering

#endif
