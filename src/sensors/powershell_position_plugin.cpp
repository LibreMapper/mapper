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


QGeoAreaMonitorSource* PowershellPositionPlugin::areaMonitor(QObject* /* parent */, const QVariantMap& parameters)
{
	Q_UNUSED(parameters)
	return nullptr;
}

QGeoPositionInfoSource* PowershellPositionPlugin::positionInfoSource(QObject* parent, const QVariantMap& parameters)
{
	Q_UNUSED(parameters)
	return new PowershellPositionSource(parent);
}

QGeoSatelliteInfoSource* PowershellPositionPlugin::satelliteInfoSource(QObject* /* parent */, const QVariantMap& parameters)
{
	Q_UNUSED(parameters)
	return nullptr;
}


}  // namespace LibreMapper
