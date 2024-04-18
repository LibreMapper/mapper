/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_POWERSHELL_POSITION_PLUGIN_H
#define LIBREMAPPER_POWERSHELL_POSITION_PLUGIN_H

#include <QGeoPositionInfoSourceFactory>
#include <QObject>
#include <QString>

class QGeoAreaMonitorSource;
class QGeoPositionInfoSource;
class QGeoSatelliteInfoSource;

namespace LibreMapper
{

/**
 * A plugin for properly registering PowershellPositionSource.
 */
class PowershellPositionPlugin : public QObject, public QGeoPositionInfoSourceFactory 
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.qt.position.sourcefactory/6.0"
	                  FILE "powershell_position_plugin.json")
	Q_INTERFACES(QGeoPositionInfoSourceFactory)
public:
	PowershellPositionPlugin(QObject* parent = nullptr);
	PowershellPositionPlugin(const PowershellPositionPlugin&) = delete;
	PowershellPositionPlugin(PowershellPositionPlugin&&) = delete;
	PowershellPositionPlugin& operator=(const PowershellPositionPlugin&) = delete;
	PowershellPositionPlugin&& operator=(PowershellPositionPlugin&&) = delete;
	~PowershellPositionPlugin() override;
	QGeoAreaMonitorSource* areaMonitor(QObject* parent, const QVariantMap &parameters) override;
	QGeoPositionInfoSource* positionInfoSource(QObject* parent, const QVariantMap &parameters) override;
	QGeoSatelliteInfoSource* satelliteInfoSource(QObject* parent, const QVariantMap &parameters) override;
};


}  // namespace LibreMapper

#endif  // LIBREMAPPER_POWERSHELL_POSITION_PLUGIN_H
