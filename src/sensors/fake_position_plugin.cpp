/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "fake_position_plugin.h"

#include "fake_position_source.h"


namespace LibreMapper
{

FakePositionPlugin::FakePositionPlugin(QObject* parent)
: QObject(parent)
{}

FakePositionPlugin::~FakePositionPlugin() = default;


QGeoAreaMonitorSource* FakePositionPlugin::areaMonitor(QObject* /* parent */, const QVariantMap& parameters)
{
	Q_UNUSED(parameters)
	return nullptr;
}

QGeoPositionInfoSource* FakePositionPlugin::positionInfoSource(QObject* parent, const QVariantMap& parameters)
{
	Q_UNUSED(parameters)
	return new FakePositionSource(parent);
}

QGeoSatelliteInfoSource* FakePositionPlugin::satelliteInfoSource(QObject* /* parent */, const QVariantMap& parameters)
{
	Q_UNUSED(parameters)
	return nullptr;
}


}  // namespace LibreMapper
