/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014, 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "autosave.h"

#include <QtGlobal>
#include <QLatin1String>
#include <QString>
#include <QVariant>

#include "settings.h"


namespace LibreMapper {

AutosavePrivate::AutosavePrivate(Autosave& autosave)
: document(autosave)
{
#ifdef QT_TESTLIB_LIB
	// The AutosaveTest uses a very short interval. By using a precise timer,
	// we try to avoid occasional AutosaveTest failures on macOS.
	autosave_timer.setTimerType(Qt::PreciseTimer);
#endif
	autosave_timer.setSingleShot(true);
	connect(&autosave_timer, &QTimer::timeout, this, &AutosavePrivate::autosave);
	connect(&Settings::getInstance(), &Settings::settingsChanged, this, &AutosavePrivate::settingsChanged);
	settingsChanged();
}

AutosavePrivate::~AutosavePrivate() = default;

void AutosavePrivate::settingsChanged()
{
	// Normally, the autosave interval can be stored as an integer.
	// It is loaded as a double here to allow for faster unit testing.
	autosave_interval = qRound(Settings::getInstance().getSetting(Settings::General_AutosaveInterval).toDouble() * 60000);
	if (autosave_interval < 1000)
	{
		// stop autosave
		autosave_interval = 0;
		autosave_timer.stop();
	}
	else if (autosave_needed && !autosave_timer.isActive())
	{
		// start autosave
		autosave_timer.setInterval(autosave_interval);
		autosave_timer.start();
	}
}

bool AutosavePrivate::autosaveNeeded() const
{
	return autosave_needed;
}

void AutosavePrivate::setAutosaveNeeded(bool needed)
{
	autosave_needed = needed;
	if (autosave_interval)
	{
		// autosaving enabled
		if (autosave_needed && !autosave_timer.isActive())
		{
			autosave_timer.setInterval(autosave_interval);
			autosave_timer.start();
		}
		else if (!autosave_needed && autosave_timer.isActive())
		{
			autosave_timer.stop();
		}
	}
}

void AutosavePrivate::autosave()
{
	Autosave::AutosaveResult result = document.autosave();
	if (autosave_interval)
	{
		switch (result)
		{
		case Autosave::TemporaryFailure:
			autosave_timer.setInterval(5000);
			autosave_timer.start();
			return;
		case Autosave::Success:
		case Autosave::PermanentFailure:
			autosave_timer.setInterval(autosave_interval);
			autosave_timer.start();
			return;
		}
		Q_UNREACHABLE();
	}
}



// ### Autosave ###

Autosave::Autosave()
: autosave_controller(*this)
{
	// nothing else
}

Autosave::~Autosave() = default;

QString Autosave::autosavePath(const QString &path) const
{
	return path + QLatin1String(".autosave");
}

void Autosave::setAutosaveNeeded(bool needed)
{
	autosave_controller.setAutosaveNeeded(needed);
}

bool Autosave::autosaveNeeded() const
{
	return autosave_controller.autosaveNeeded();
}


}  // namespace LibreMapper

#include "moc_autosave.cpp"
