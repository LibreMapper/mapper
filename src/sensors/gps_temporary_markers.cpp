/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "gps_temporary_markers.h"

#include <Qt>
#include <QtGlobal>
#include <QBrush>
#include <QPainter>
#include <QPen>
#include <QRgb>

#include "core/map_coord.h"
#include "core/map_view.h"
#include "gui/map/map_widget.h"
#include "sensors/gps_display.h"
#include "tools/tool.h"

class QPointF;


namespace LibreMapper {

GPSTemporaryMarkers::GPSTemporaryMarkers(MapWidget* widget, GPSDisplay* gps_display): QObject()
{
	this->widget = widget;
	this->gps_display = gps_display;
	recording_path = false;
	
	connect(gps_display, &GPSDisplay::mapPositionUpdated, this, &GPSTemporaryMarkers::newGPSPosition);
	
	widget->setTemporaryMarkerDisplay(this);
}

GPSTemporaryMarkers::~GPSTemporaryMarkers()
{
	widget->setTemporaryMarkerDisplay(nullptr);
}

bool GPSTemporaryMarkers::addPoint()
{
	if (!gps_display->hasValidPosition())
		return false;
	
	points.push_back(gps_display->getLatestGPSCoord());
	updateMapWidget();
	return true;
}

void GPSTemporaryMarkers::startPath()
{
	paths.push_back(std::vector< QPointF >());
	recording_path = true;
	if (gps_display->hasValidPosition())
		newGPSPosition(gps_display->getLatestGPSCoord(), gps_display->getLatestGPSCoordAccuracy());
}

void GPSTemporaryMarkers::stopPath()
{
	recording_path = false;
}

void GPSTemporaryMarkers::clear()
{
	points.clear();
	paths.clear();
	updateMapWidget();
}

void GPSTemporaryMarkers::paint(QPainter* painter)
{
	painter->save();
	widget->applyMapTransform(painter);
	float scale_factor = 1.0f / widget->getMapView()->getZoom();
	
	// Draw paths
	painter->setBrush(Qt::NoBrush);

	painter->setPen(QPen(QBrush(qRgb(255, 255, 255)), scale_factor * 0.3f));
	for (const auto& path : paths)
		painter->drawPolyline(path.data(), path.size());
	
	painter->setPen(QPen(QBrush(MapEditorTool::active_color), scale_factor * 0.2f));
	for (const auto& path : paths)
		painter->drawPolyline(path.data(), path.size());
	
	// Draw points
	painter->setPen(Qt::NoPen);
	
	painter->setBrush(QBrush(qRgb(255, 255, 255)));
	float point_radius = scale_factor * 0.5f;
	for (const auto& point : points)
		painter->drawEllipse(point, point_radius, point_radius);
	
	painter->setBrush(QBrush(MapEditorTool::inactive_color));
	point_radius = scale_factor * 0.4f;
	for (const auto& point : points)
		painter->drawEllipse(point, point_radius, point_radius);
	
	painter->restore();
}

void GPSTemporaryMarkers::newGPSPosition(const MapCoordF& coord, float accuracy)
{
	Q_UNUSED(accuracy);

	if (recording_path && ! paths.empty())
	{
		std::vector< QPointF >& path_coords = paths.back();
		path_coords.push_back(coord);
		updateMapWidget();
	}
}

void GPSTemporaryMarkers::updateMapWidget()
{
	// NOTE: could limit the updated area here
	widget->update();
}


}  // namespace LibreMapper
