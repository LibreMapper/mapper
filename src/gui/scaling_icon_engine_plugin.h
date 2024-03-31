/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_SCALING_ICON_ENGINE_PLUGIN_H
#define LIBREMAPPER_SCALING_ICON_ENGINE_PLUGIN_H

#include <QIconEnginePlugin>
#include <QObject>
#include <QString>

class QIconEngine;

namespace LibreMapper {


/**
 * This plugin provides an icon engine that can scale up icons.
 * 
 * For most of its behaviour, this plugin and the icon engine it creates rely
 * on Qt's default icon engine. It merely acts as a proxy. This is achieved by
 * registering this plugin with high priority for "PNG", but using a delegate
 * created by Qt for "BMP". In this configuration, Qt will select this plugin
 * only for PNG files.
 */
class ScalingIconEnginePlugin : public QIconEnginePlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QIconEngineFactoryInterface"
	                  FILE "scaling_icon_engine_plugin.json")
	
public:
	~ScalingIconEnginePlugin() override;
	ScalingIconEnginePlugin(QObject *parent = nullptr);
	virtual QIconEngine *create(const QString& filename = {}) override;
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_SCALING_ICON_ENGINE_H
