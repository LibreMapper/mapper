/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Thomas Schöps (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_GPS_TEMPORARY_MARKERS_H
#define OPENORIENTEERING_GPS_TEMPORARY_MARKERS_H

#include <vector>

#include <QObject>
#include <QPointF>
#include <QString>

class QPainter;
// IWYU pragma: no_forward_declare QPointF

namespace OpenOrienteering {

class MapCoordF;
class MapWidget;
class GPSDisplay;


/** Displays temporary markers recorded with GPS. */
class GPSTemporaryMarkers : public QObject
{
Q_OBJECT
public:
	GPSTemporaryMarkers(MapWidget* widget, GPSDisplay* gps_display);
	~GPSTemporaryMarkers() override;
	
	/** Returns false if no point was added due to not having a valid position yet. */
	bool addPoint();
	/** Starts recording a GPS path. */
	void startPath();
	/** Stops recording a GPS path. */
	void stopPath();
	/** Deletes all temporary markers. */
	void clear();
	
	/// This is called from the MapWidget drawing code to draw the markers.
	void paint(QPainter* painter);
	
public slots:
	void newGPSPosition(const OpenOrienteering::MapCoordF& coord, float accuracy);
	
private:
	void updateMapWidget();
	
	bool recording_path;
	std::vector< QPointF > points;
	std::vector< std::vector< QPointF > > paths;
	GPSDisplay* gps_display;
	MapWidget* widget;
};


}  // namespace OpenOrienteering

#endif
