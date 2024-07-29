/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2021 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_IOF_COURSE_EXPORT_H
#define LIBREMAPPER_IOF_COURSE_EXPORT_H

#include <vector>

#include "fileformats/file_import_export.h"

class QString;
class QXmlStreamWriter;

namespace LibreMapper {

class LatLon;
class Map;
class MapView;
class MapCoord;
class PathObject;
class SimpleCourseExport;


/**
 * This class generates IOF Interface Standard 3.0 course files.
 * 
 * This export handles a single path object and outputs placemarks for start
 * (S1), finish (F1), and controls in between. Event name, course name, and
 * the code number of the first control are taken from transient map properties
 * in collaboration with the SimpleCourseExport class.
 */
class IofCourseExport : public Exporter
{
public:
	static QString formatDescription();
	static QString filenameExtension();
	
	~IofCourseExport();
	
	IofCourseExport(const QString& path, const Map* map, const MapView* view);
	
protected:
	bool exportImplementation() override;
	
	void writeXml(const PathObject& object);
	
	void writeControls(const std::vector<MapCoord>& coords);
	
	void writeCourse(const std::vector<MapCoord>& coords);
	
	void writeControl(const MapCoord& coord, const QString& id);
	
	void writeCourseControl(const QString& type, const QString& id);
	
	void writePosition(const LatLon& latlon);
	
private:
	QXmlStreamWriter* xml = nullptr;
	SimpleCourseExport* simple_course = nullptr;
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_IOF_COURSE_EXPORT_H
