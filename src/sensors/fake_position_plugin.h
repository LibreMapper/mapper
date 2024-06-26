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

namespace LibreMapper
{

/**
 * A plugin for properly registering FakePositionSource.
 */
class FakePositionPlugin : public QObject, public QGeoPositionInfoSourceFactory 
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.qt.position.sourcefactory/6.0"
					  FILE "fake_position_plugin.json")
	Q_INTERFACES(QGeoPositionInfoSourceFactory)
public:
	FakePositionPlugin(QObject* parent = nullptr);
	FakePositionPlugin(const FakePositionPlugin&) = delete;
	FakePositionPlugin(FakePositionPlugin&&) = delete;
	FakePositionPlugin& operator=(const FakePositionPlugin&) = delete;
	FakePositionPlugin&& operator=(FakePositionPlugin&&) = delete;
	~FakePositionPlugin() override;
	QGeoAreaMonitorSource* areaMonitor(QObject* parent, const QVariantMap &parameters) override;
	QGeoPositionInfoSource* positionInfoSource(QObject* parent, const QVariantMap &parameters) override;
	QGeoSatelliteInfoSource* satelliteInfoSource(QObject* parent, const QVariantMap &parameters) override;
};


}  // namespace LibreMapper

#endif  // LIBREMAPPER_FAKE_POSITION_PLUGIN_H
