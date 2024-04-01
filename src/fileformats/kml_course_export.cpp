/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020-2021 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "kml_course_export.h"

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


namespace OpenOrienteering {

// static
QString KmlCourseExport::formatDescription()
{
	return OpenOrienteering::ImportExport::tr("KML");
}

// static
QString KmlCourseExport::filenameExtension()
{
	return QStringLiteral("kml");
}


KmlCourseExport::~KmlCourseExport() = default;

KmlCourseExport::KmlCourseExport(const QString& path, const Map* map, const MapView* view)
: Exporter(path, map, view)
{}


// override
bool KmlCourseExport::exportImplementation()
{
	auto course_export = SimpleCourseExport(*map);
	auto const* const object = course_export.findObjectForExport();
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
	writeKml(*object);
	xml = nullptr;
	
	simple_course = nullptr;
	return true;
}


void KmlCourseExport::writeKml(const PathObject& object)
{
	auto const stamp = QDateTime::currentDateTime();
	xml->writeDefaultNamespace(QLatin1String("http://www.opengis.net/kml/2.2"));
	xml->writeComment(QLatin1String("Generator: OpenOrienteering Mapper " APP_VERSION));
	xml->writeComment(QLatin1String("Created:   ") + stamp.toString(Qt::ISODate));
	
	XmlElementWriter kml(*xml, QLatin1String("kml"));
	{
		XmlElementWriter document(*xml, QLatin1String("Document"));
		xml->writeTextElement(QLatin1String("name"), simple_course->eventName());
		{
			XmlElementWriter folder(*xml, QLatin1String("Folder"));
			xml->writeTextElement(QLatin1String("name"), simple_course->courseName());
			xml->writeTextElement(QLatin1String("open"), QLatin1String("1"));
			writeKmlPlacemarks(object.getRawCoordinateVector());
		}
	}
}

void KmlCourseExport::writeKmlPlacemarks(const std::vector<MapCoord>& coords)
{
	auto next = [](auto current) {
		return current + (current->isCurveStart() ? 3 : 1);
	};
	
	writeKmlPlacemark(coords.front(), QLatin1String("S1"), QLatin1String("Start"));
	auto code_number = simple_course->firstCode();
	for (auto current = next(coords.begin()); current != coords.end() - 1; current = next(current))
	{
		auto const name = QString::number(code_number);
		writeKmlPlacemark(*current, name, QLatin1String("Control ") + name);
		++code_number;
	}
	writeKmlPlacemark(coords.back(), QLatin1String("F1"), QLatin1String("Finish"));
}

void KmlCourseExport::writeKmlPlacemark(const MapCoord& coord, const QString& name, const QString& description)
{
	XmlElementWriter placemark(*xml, QLatin1String("Placemark"));
	xml->writeTextElement(QLatin1String("name"), name);
	xml->writeTextElement(QLatin1String("description"), description);
	{
		XmlElementWriter point(*xml, QLatin1String("Point"));
		writeCoordinates(map->getGeoreferencing().toGeographicCoords(MapCoordF(coord)));
	}
}

void KmlCourseExport::writeCoordinates(const LatLon& latlon)
{
	XmlElementWriter coordinates(*xml, QLatin1String("coordinates"));
	xml->writeCharacters(QString::number(latlon.longitude(), 'f', 7));
	xml->writeCharacters(QLatin1String(","));
	xml->writeCharacters(QString::number(latlon.latitude(), 'f', 7));
	xml->writeCharacters(QLatin1String(",0"));
}


}  // namespace OpenOrienteering
