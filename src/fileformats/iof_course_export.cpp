/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2021 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "iof_course_export.h"

#include <Qt>
#include <QDateTime>
#include <QLatin1String>
#include <QString>
#include <QXmlStreamWriter>

#include "mapper_config.h"
#include "core/georeferencing.h"
#include "core/latlon.h"
#include "core/map.h"
#include "core/map_coord.h"
#include "core/objects/object.h"
#include "fileformats/simple_course_export.h"
#include "util/xml_stream_util.h"


namespace LibreMapper {

// static
QString IofCourseExport::formatDescription()
{
	return LibreMapper::ImportExport::tr("IOF Data Standard 3.0");
}

// static
QString IofCourseExport::filenameExtension()
{
	return QStringLiteral("xml");
}


IofCourseExport::~IofCourseExport() = default;

IofCourseExport::IofCourseExport(const QString& path, const Map* map, const MapView* view)
: Exporter(path, map, view)
{}


bool IofCourseExport::exportImplementation()
{
	SimpleCourseExport course_export(*map);
	auto* object = course_export.findObjectForExport();
	
	if (!course_export.canExport(object))
	{
		addWarning(course_export.errorString());
		return false;
	}
	
	simple_course = &course_export;
	
	QXmlStreamWriter writer(device());
	writer.setAutoFormatting(true);
	xml = &writer;
	xml->writeStartDocument();
	writeXml(*object);
	xml = nullptr;
	
	simple_course = nullptr;
	return true;
}


void IofCourseExport::writeXml(const PathObject& object)
{
	auto const stamp = QDateTime::currentDateTime();
	xml->writeDefaultNamespace(QLatin1String("http://www.orienteering.org/datastandard/3.0"));
	
	XmlElementWriter course_data(*xml, QLatin1String("CourseData"));
	course_data.writeAttribute(QLatin1String("iofVersion"), QLatin1String("3.0"));
	course_data.writeAttribute(QLatin1String("creator"), QLatin1String("OpenOrienteering Mapper " APP_VERSION));
	course_data.writeAttribute(QLatin1String("createTime"), stamp.toString(Qt::ISODate));
	{
		XmlElementWriter event(*xml, QLatin1String("Event"));
		xml->writeTextElement(QLatin1String("Name"), simple_course->eventName());
	}
	{
		XmlElementWriter event(*xml, QLatin1String("RaceCourseData"));
		writeControls(object.getRawCoordinateVector());
		writeCourse(object.getRawCoordinateVector());
	}
}

void IofCourseExport::writeControls(const std::vector<MapCoord>& coords)
{
	auto next = [](auto current) {
		return current + (current->isCurveStart() ? 3 : 1);
	};
	
	writeControl(coords.front(), QLatin1String("S1"));
	auto code_number = simple_course->firstCode();
	for (auto current = next(coords.begin()); current != coords.end() - 1; current = next(current))
	{
		auto const name = QString::number(code_number);
		writeControl(*current, name);
		++code_number;
	}
	writeControl(coords.back(), QLatin1String("F1"));
}

void IofCourseExport::writeCourse(const std::vector<MapCoord>& coords)
{
	auto next = [](auto current) {
		return current + (current->isCurveStart() ? 3 : 1);
	};
	
	XmlElementWriter event(*xml, QLatin1String("Course"));
	xml->writeTextElement(QLatin1String("Name"), simple_course->courseName());
	writeCourseControl(QLatin1String("Start"), QLatin1String("S1"));
	auto code_number = simple_course->firstCode();
	for (auto current = next(coords.begin()); current != coords.end() - 1; current = next(current))
	{
		auto const name = QString::number(code_number);
		writeCourseControl(QLatin1String("Control"), name);
		++code_number;
	}
	writeCourseControl(QLatin1String("Finish"), QLatin1String("F1"));
}

void IofCourseExport::writeControl(const MapCoord& coord, const QString& id)
{
	XmlElementWriter control(*xml, QLatin1String("Control"));
	xml->writeTextElement(QLatin1String("Id"), id);
	writePosition(map->getGeoreferencing().toGeographicCoords(MapCoordF(coord)));
}

void IofCourseExport::writeCourseControl(const QString& type, const QString& id)
{
	XmlElementWriter course_control(*xml, QLatin1String("CourseControl"));
	course_control.writeAttribute(QLatin1String("type"), type);
	xml->writeTextElement(QLatin1String("Control"), id);
}

void IofCourseExport::writePosition(const LatLon& latlon)
{
	XmlElementWriter position(*xml, QLatin1String("Position"));
	position.writeAttribute(QLatin1String("lng"), latlon.longitude(), 7);
	position.writeAttribute(QLatin1String("lat"), latlon.latitude(), 7);
}


}  // namespace LibreMapper
