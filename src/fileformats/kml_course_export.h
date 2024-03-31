/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020-2021 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_KML_EXPORT_EXPORT_H
#define LIBREMAPPER_KML_EXPORT_EXPORT_H

#include <vector>

#include "fileformats/file_import_export.h"

class QString;
class QXmlStreamWriter;

namespace LibreMapper {

class LatLon;
class Map;
class MapCoord;
class MapView;
class PathObject;
class SimpleCourseExport;


/**
 * This class generates KML course files for MapRunF.
 * 
 * This export handles a single path object and outputs placemarks for start
 * (S1), finish (F1), and controls in between. Event name, course name, and
 * the code number of the first control are taken from transient map properties
 * in collaboration with the SimpleCourseExport class.
 */
class KmlCourseExport : public Exporter
{
public:
	static QString formatDescription();
	static QString filenameExtension();
	
	~KmlCourseExport();
	
	KmlCourseExport(const QString& path, const Map* map, const MapView* view);
	
protected:
	bool exportImplementation() override;
	
	void writeKml(const PathObject& object);
	
	void writeKmlPlacemarks(const std::vector<MapCoord>& coords);
	
	void writeKmlPlacemark(const MapCoord& coord, const QString& name, const QString& description);
	
	void writeCoordinates(const LatLon& latlon);
	
private:
	QXmlStreamWriter* xml = nullptr;
	SimpleCourseExport* simple_course = nullptr;
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_KML_EXPORT_EXPORT_H
