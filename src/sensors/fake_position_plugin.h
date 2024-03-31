/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_FAKE_POSITION_PLUGIN_H
#define LIBREMAPPER_FAKE_POSITION_PLUGIN_H

#include <QGeoPositionInfoSourceFactory>
#include <QObject>
#include <QString>

class QGeoAreaMonitorSource;
class QGeoPositionInfoSource;
class QGeoSatelliteInfoSource;

namespace OpenOrienteering
{

/**
 * A plugin for properly registering FakePositionSource.
 */
class FakePositionPlugin : public QObject, public QGeoPositionInfoSourceFactory 
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.qt.position.sourcefactory/5.0"
	                  FILE "fake_position_plugin.json")
	Q_INTERFACES(QGeoPositionInfoSourceFactory)
public:
	FakePositionPlugin(QObject* parent = nullptr);
	FakePositionPlugin(const FakePositionPlugin&) = delete;
	FakePositionPlugin(FakePositionPlugin&&) = delete;
	FakePositionPlugin& operator=(const FakePositionPlugin&) = delete;
	FakePositionPlugin&& operator=(FakePositionPlugin&&) = delete;
	~FakePositionPlugin() override;
	QGeoAreaMonitorSource* areaMonitor(QObject* parent) override;
	QGeoPositionInfoSource* positionInfoSource(QObject* parent) override;
	QGeoSatelliteInfoSource* satelliteInfoSource(QObject* parent) override;
};


}  // namespace OpenOrienteering

#endif  // LIBREMAPPER_FAKE_POSITION_PLUGIN_H
