/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_NMEA_PLUGIN_H
#define LIBREMAPPER_NMEA_PLUGIN_H

#include <QGeoPositionInfoSourceFactory>
#include <QObject>
#include <QString>

class QGeoAreaMonitorSource;
class QGeoPositionInfoSource;
class QGeoSatelliteInfoSource;

namespace LibreMapper
{

/**
 * A plugin for using QNmeaPositionInfoSource without QSerialPort.
 */
class NmeaPositionPlugin : public QObject, public QGeoPositionInfoSourceFactory 
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.qt.position.sourcefactory/5.0"
	                  FILE "nmea_position_plugin.json")
	Q_INTERFACES(QGeoPositionInfoSourceFactory)
	
public:
	~NmeaPositionPlugin() override;
	NmeaPositionPlugin(QObject* parent = nullptr);
	
	NmeaPositionPlugin(const NmeaPositionPlugin&) = delete;
	NmeaPositionPlugin(NmeaPositionPlugin&&) = delete;
	NmeaPositionPlugin& operator=(const NmeaPositionPlugin&) = delete;
	NmeaPositionPlugin&& operator=(NmeaPositionPlugin&&) = delete;
	
	QGeoAreaMonitorSource* areaMonitor(QObject* parent) override;
	QGeoPositionInfoSource* positionInfoSource(QObject* parent) override;
	QGeoSatelliteInfoSource* satelliteInfoSource(QObject* parent) override;
};


}  // namespace LibreMapper

#endif  // LIBREMAPPER_NMEA_PLUGIN_H
