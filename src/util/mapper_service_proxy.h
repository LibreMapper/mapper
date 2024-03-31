/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_MAPPER_SERVICE_PROXY_H
#define LIBREMAPPER_MAPPER_SERVICE_PROXY_H

#include <QPointer>

class QWidget;

namespace LibreMapper {

/**
 * A class which helps to run a service alongside the application.
 * 
 * The service is started by setting a non-null active window, and stopped
 * by setting the active window to null.
 * 
 * On Android, this utility runs a foregound service with a notification
 * showing the app name and the file name. Running a foreground service
 * increases the app's importance with regard to power management and
 * continuous location access, and it makes the user aware of the app
 * when it is in the background.
 * 
 * On other platforms, this class does nothing at the moment.
 */
class MapperServiceProxy
{
public:
	MapperServiceProxy() = default;
	~MapperServiceProxy();
	
	QWidget* activeWindow() { return active_window.data(); }
	void setActiveWindow(QWidget* widget);
	
private:
	void startService();
	void stopService();
	
	QPointer<QWidget> active_window;
	
	Q_DISABLE_COPY(MapperServiceProxy)
};

}  // namespace LibreMapper

#endif
