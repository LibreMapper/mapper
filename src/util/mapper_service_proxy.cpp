/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "mapper_service_proxy.h"

#include <QWidget>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

#include "gui/main_window.h"


namespace OpenOrienteering {

#ifdef Q_OS_ANDROID

namespace {

auto const foreground_service = QStringLiteral("android.permission.FOREGROUND_SERVICE");

/**
 * A callback which handles Android permission request results.
 * 
 * In addition to the MapperServiceProxy and it is member function, this unit
 * takes a QPointer to the target window which also serves as an indicator that
 * the proxy object still exists at the time the functor is called.
 */
struct MapperServiceProxyCallback
{
	MapperServiceProxy* proxy;
	void (MapperServiceProxy::*start_service)();
	QPointer<QWidget> window;
	
	void operator()(const QtAndroid::PermissionResultMap &results)
	{
		if (!window || window != proxy->activeWindow())
			return;
		if (results[foreground_service] == QtAndroid::PermissionResult::Granted)
			(proxy->*start_service)();
	}
};

}

#endif


MapperServiceProxy::~MapperServiceProxy()
{
	setActiveWindow(nullptr);
}


void MapperServiceProxy::setActiveWindow(QWidget* window)
{
	if (active_window == window)
		return;
	
	if (active_window != nullptr)
		stopService();
		
	active_window = window;
		
	if (active_window == nullptr)
		return;
	
#ifdef Q_OS_ANDROID
	if (QtAndroid::androidSdkVersion() >= 28)
	{
		if (QtAndroid::checkPermission(foreground_service) != QtAndroid::PermissionResult::Granted)
		{
			auto callback = MapperServiceProxyCallback{this, &MapperServiceProxy::startService, window};
			QtAndroid::requestPermissions({foreground_service}, callback);
			return;
		}
	}
#endif
	
	startService();
}


void MapperServiceProxy::startService()
{
	Q_ASSERT(active_window);
	
#ifdef Q_OS_ANDROID
	auto const file_path = active_window->windowFilePath();
	auto const prefix_length = file_path.lastIndexOf(QLatin1Char('/')) + 1;
	QAndroidJniObject java_string = QAndroidJniObject::fromString(file_path.mid(prefix_length));
	QAndroidJniObject::callStaticMethod<void>("org/openorienteering/mapper/MapperActivity",
	                                          "startService",
	                                          "(Ljava/lang/String;)V",
	                                          java_string.object<jstring>());
#endif
}


void MapperServiceProxy::stopService()
{
	Q_ASSERT(active_window);
	
#ifdef Q_OS_ANDROID
	QAndroidJniObject::callStaticMethod<void>("org/openorienteering/mapper/MapperActivity",
	                                          "stopService",
	                                          "()V");
#endif
}

}  // namespace OpenOrienteering
