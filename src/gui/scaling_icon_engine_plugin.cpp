/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "scaling_icon_engine_plugin.h"

#include <QIconEnginePlugin>

#include "scaling_icon_engine.h"

namespace OpenOrienteering {

ScalingIconEnginePlugin::~ScalingIconEnginePlugin() = default;

ScalingIconEnginePlugin::ScalingIconEnginePlugin(QObject* parent)
: QIconEnginePlugin(parent)
{}

QIconEngine* ScalingIconEnginePlugin::create(const QString& filename)
{
	return new ScalingIconEngine(filename);
}


}  // namespace OpenOrienteering
