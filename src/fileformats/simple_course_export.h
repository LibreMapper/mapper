/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2021 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_SIMPLE_COURSE_EXPORT_H
#define OPENORIENTEERING_SIMPLE_COURSE_EXPORT_H

#include <QCoreApplication>
#include <QString>

namespace OpenOrienteering {

class Map;
class PathObject;

/**
 * This class provides utility functions for exporting simple course files.
 * 
 * Simple course files can be created either from a single selected path object,
 * or from the single path object contained in the map's single map part.
 */
class SimpleCourseExport
{
	Q_DECLARE_TR_FUNCTIONS(OpenOrienteering::SimpleCourseExport)
	
public:
	static QString defaultEventName();
	static QString defaultCourseName();
	static int defaultFirstCode() noexcept;
	
	~SimpleCourseExport() = default;
	
	SimpleCourseExport(const Map& map) : map{map} {};
	
	SimpleCourseExport(const SimpleCourseExport&) = default;
	
	
	bool canExport();
	
	bool canExport(const PathObject* object);
	
	const PathObject* findObjectForExport() const;
	
	
	QString eventName() const;
	
	QString courseName() const;
	
	int firstCode() const;
	
	void setProperties(Map& map, const QString& event_name, const QString& course_name, int first_code);
	
	
	QString errorString() const { return error_string; };
	
private:
	const Map& map;
	QString error_string;
	
};


}  // namespace OpenOrienteering

#endif // OPENORIENTEERING_SIMPLE_COURSE_EXPORT_H
