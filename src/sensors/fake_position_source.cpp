/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2018-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifdef QT_POSITIONING_LIB

#include "fake_position_source.h"

#include <algorithm>

#include <QtGlobal>
#include <QtMath>
#include <QDateTime>
#include <QGeoCoordinate>
#include <QGeoPositionInfo>
#include <QTime>
#include <QTimerEvent>


namespace OpenOrienteering
{

QGeoCoordinate FakePositionSource::initial_reference  {};

// static
void FakePositionSource::setReferencePoint(const QGeoCoordinate &reference)
{
	FakePositionSource::initial_reference = reference;
}


FakePositionSource::~FakePositionSource() = default;

FakePositionSource::FakePositionSource(QObject* object)
: FakePositionSource(initial_reference, object)
{}

FakePositionSource::FakePositionSource(const QGeoCoordinate& reference, QObject* object)
: QGeoPositionInfoSource(object)
, reference(reference)
{}


QGeoPositionInfoSource::Error FakePositionSource::error() const
{
	return NoError;
}

QGeoPositionInfo FakePositionSource::lastKnownPosition(bool /*fromSatellitePositioningMethodsOnly*/) const
{
	return info;
}

int FakePositionSource::minimumUpdateInterval() const
{
	return 500;
}

QGeoPositionInfoSource::PositioningMethods FakePositionSource::supportedPositioningMethods() const
{
	return SatellitePositioningMethods;
}

void FakePositionSource::requestUpdate(int /*timeout*/)
{
	const auto now = QDateTime::currentDateTime();
	const auto offset = now.time().msecsSinceStartOfDay() / qreal(20000);
	const auto position = QGeoCoordinate {
	                      reference.latitude() + 0.001 * qSin(offset),
	                      reference.longitude() + 0.001 * qCos(offset * 1.1),
	                      reference.altitude() + 10 * qSin(1 + offset / 10) };
	info = {position, now};
	info.setAttribute(QGeoPositionInfo::HorizontalAccuracy, 12 + 7 * qSin(2 + offset));
	emit positionUpdated(info);
}

void FakePositionSource::startUpdates()
{
	if (!timer_id)
	{
		timer_id = startTimer(std::max(minimumUpdateInterval(), updateInterval()));
	}
}

void FakePositionSource::stopUpdates()
{
	if (timer_id)
	{
		killTimer(timer_id);
		timer_id = 0;
	}
}

void FakePositionSource::timerEvent(QTimerEvent* e)
{
	if (e->timerId() == timer_id)
	{
		requestUpdate(0);
	}
}


}  // namespace OpenOrienteering


#endif // QT_POSITIONING_LIB
