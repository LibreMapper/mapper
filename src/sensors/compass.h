/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_COMPASS_H
#define LIBREMAPPER_COMPASS_H

#include <memory>

#include <QObject>
#include <QString>

class QMetaMethod;

namespace LibreMapper {

class CompassPrivate;


/** Provides access to the device's compass. Singleton class. */
class Compass : public QObject
{
Q_OBJECT
friend class CompassPrivate;
public:
	~Compass() override;
	
	/** Singleton accessor method. Constructs the object on first use. */
	static Compass& getInstance();
	
	/** Adds a reference. If this is the first one, initializes the compass. */
	void startUsage();
	
	/** Dereferences compass usage. */
	void stopUsage();
	
	/** Returns the most recent azimuth value
	 *  (in degrees clockwise from north; updated approx. every 30 milliseconds). */
	float getCurrentAzimuth();
	
	/** Connects to the azimuthChanged(float azimuth_degrees) signal. This ensures to use a queued
	 *  connection, which is important because the data provider runs on another
	 *  thread. Updates are delivered approx. every 30 milliseconds. */
	void connectToAzimuthChanges(const QObject* receiver, const char* slot);
	
	/** Disconnects the given receiver from azimuth changes. */
	void disconnectFromAzimuthChanges(const QObject* receiver);
	
signals:
	/** Emitted regularly with the current azimuth value (in degrees).
	 *  Preferably use connectToAzimuthChanges() to connect to this signal. */
	void azimuthChanged(float azimuth);
	
protected:
	void connectNotify(const QMetaMethod& signal) override;
	
	void disconnectNotify(const QMetaMethod& signal) override;
	
private:
	void emitAzimuthChanged(float value);
	
	std::unique_ptr<CompassPrivate> p;
	int reference_counter = 0;
};


}  // namespace LibreMapper

#endif
