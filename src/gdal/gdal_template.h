/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_GDAL_TEMPLATE_H
#define LIBREMAPPER_GDAL_TEMPLATE_H

#include <vector>

#include <QString>

#include "templates/template.h"
#include "templates/template_image.h"

class QByteArray;

namespace OpenOrienteering {

class Map;


/**
 * Support for geospatial raster data.
 */
class GdalTemplate : public TemplateImage
{
public:
	static bool canRead(const QString& path);
	
	static const std::vector<QByteArray>& supportedExtensions();
	
	static const char* applyCornerPassPointsProperty();
	
	GdalTemplate(const QString& path, Map* map);
	~GdalTemplate() override;
	
protected:
	GdalTemplate(const GdalTemplate& proto);
	GdalTemplate* duplicate() const override;
	
public:
	const char* getTemplateType() const override;
	
	LookupResult tryToFindTemplateFile(const QString& map_path) override;
	
	bool fileExists() const override;
	
protected:
	bool loadTemplateFileImpl() override;
	
	bool applyCornerPassPoints();
};


}  // namespace OpenOrienteering

#endif // LIBREMAPPER_GDAL_TEMPLATE_H
