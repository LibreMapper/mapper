/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "powershell_position_plugin.h"

#include "powershell_position_source.h"


namespace LibreMapper
{

PowershellPositionPlugin::PowershellPositionPlugin(QObject* parent)
: QObject(parent)
{}

PowershellPositionPlugin::~PowershellPositionPlugin() = default;


QGeoAreaMonitorSource* PowershellPositionPlugin::areaMonitor(QObject* /* parent */)
{
	return nullptr;
}

QGeoPositionInfoSource* PowershellPositionPlugin::positionInfoSource(QObject* parent)
{
	return new PowershellPositionSource(parent);
}

QGeoSatelliteInfoSource* PowershellPositionPlugin::satelliteInfoSource(QObject* /* parent */)
{
	return nullptr;
}


}  // namespace LibreMapper
