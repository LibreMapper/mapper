/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "mapper_resource.h"

#include <QDir>
#include <QLatin1String>
#include <QString>
#include <QStringList>

#include "mapper_config.h"


namespace OpenOrienteering {

namespace MapperResource {

void setSeachPaths()
{
	QStringList data_paths;
	data_paths.reserve(3);
	
	QStringList doc_paths;
	doc_paths.reserve(3);
	
#if defined(MAPPER_DEVELOPMENT_BUILD) && defined(MAPPER_DEVELOPMENT_RES_DIR)
	// Use the directory where Mapper is built during development, 
	// even for the unit tests located in other directories.
	auto build_dir = QString::fromLocal8Bit(MAPPER_DEVELOPMENT_RES_DIR);
	data_paths.append(build_dir);
	doc_paths.append(build_dir + QLatin1String("/doc/licensing"));
	doc_paths.append(build_dir + QLatin1String("/doc/manual"));
#endif
	
#if defined(Q_OS_MACOS)
	// Mac OS X: load resources from the Resources directory of the bundle
	auto app_dir = QDir { QCoreApplication::applicationDirPath() };
	auto resources_dir = app_dir.absoluteFilePath(QLatin1String("../Resources"));
	data_paths.append(resources_dir);
	doc_paths.append(resources_dir + QLatin1String("/doc"));
#elif defined(Q_OS_WIN)
	// Windows: load resources from the application directory
	auto app_dir = QDir(QCoreApplication::applicationDirPath()).absolutePath();
	data_paths.append(app_dir);
	doc_paths.append(app_dir + QLatin1String("/doc"));
#elif defined(Q_OS_ANDROID)
	// Android: load resources from the assets directory
	auto assets = QString::fromLatin1("assets:");
	data_paths.append(assets);
	doc_paths.append(assets + QLatin1String("/doc"));
#else
	data_paths.append(QString::fromLatin1(MAPPER_DATA_DESTINATION));
	doc_paths.append(QString::fromLatin1(MAPPER_ABOUT_DESTINATION));
#endif
	
	// General default path: Qt resource system
	auto resources = QString{ QLatin1String{":"} };
	data_paths.append(resources);
	doc_paths.append(resources + QLatin1String("/doc"));
	
	QDir::setSearchPaths(QLatin1String("data"), data_paths);
	QDir::setSearchPaths(QLatin1String("doc"), doc_paths);
}


}  // namespace MapperResource


}  // namespace OpenOrienteering
