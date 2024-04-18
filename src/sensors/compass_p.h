/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_COMPASS_P_H
#define LIBREMAPPER_COMPASS_P_H

#include <QAccelerometer>
#include <QGyroscopeFilter>
#include <QMagnetometer>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>


namespace LibreMapper
{
class Compass;
	        
class CompassPrivate : public QGyroscopeFilter
{
public:
	CompassPrivate(Compass* compass);	
	~CompassPrivate() override;
	
	void enable(bool enabled);
	float getLatestAzimuth();
	bool filter(QGyroscopeReading* reading) override;
	
	
private:
	class SensorThread : public QThread
	{
	// Meta object features not supported for nested classes
	// Q_OBJECT
	public:
		SensorThread(CompassPrivate* p);
		float fuseOrientationCoefficient(float gyro, float acc_mag);
		void filter();
		void run() override;
		
		QMutex wait_mutex;
		QWaitCondition condition;
		bool keep_running;
		CompassPrivate* p;
	};
	
	QAccelerometer accelerometer;
	QMagnetometer magnetometer;
	QGyroscope gyroscope;
	bool gyro_available;
	float gyro_orientation[3];
	float gyro_rotation_matrix[9];
	quint64 last_gyro_timestamp;
	bool gyro_orientation_initialized;
	QMutex gyro_mutex;
	
	SensorThread thread;
	Compass* compass;
	
	bool enabled;
	float latest_azimuth;
};

} // namespace LibreMapper

#endif // LIBREMAPPER_COMPASS_P_H
