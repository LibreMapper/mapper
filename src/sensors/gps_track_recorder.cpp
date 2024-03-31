/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Thomas Schöps (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "gps_track_recorder.h"

#include <QDateTime>
#include <QtGlobal>

#include "core/latlon.h"
#include "core/map.h"
#include "core/map_view.h"
#include "core/track.h"
#include "gui/map/map_widget.h"
#include "sensors/gps_display.h"
#include "templates/template_track.h"


namespace LibreMapper {

GPSTrackRecorder::GPSTrackRecorder(GPSDisplay* gps_display, TemplateTrack* target_template, int draw_update_interval_milliseconds, MapWidget* widget)
 : QObject()
{
	this->gps_display = gps_display;
	this->target_template = target_template;
	this->widget = widget;
	
	track_changed_since_last_update = false;
	is_active = true;
	
	// Start with a new segment
	target_template->getTrack().finishCurrentSegment();
	
	connect(gps_display, &GPSDisplay::latLonUpdated, this, &GPSTrackRecorder::newPosition);
	connect(gps_display, &GPSDisplay::positionUpdatesInterrupted, this, &GPSTrackRecorder::positionUpdatesInterrupted);
	connect(target_template->getMap(), &Map::templateDeleted, this, &GPSTrackRecorder::templateDeleted);
	
	if (draw_update_interval_milliseconds > 0)
	{
		connect(&draw_update_timer, &QTimer::timeout, this, &GPSTrackRecorder::drawUpdate);
		draw_update_timer.start(draw_update_interval_milliseconds);
	}
}

void GPSTrackRecorder::newPosition(double latitude, double longitude, double altitude, float accuracy)
{
	const auto new_point = TrackPoint {
		LatLon(latitude, longitude),
		QDateTime::currentDateTimeUtc(),
		static_cast<float>(altitude),
		accuracy
	};
	target_template->getTrack().appendTrackPoint(new_point);
	target_template->setHasUnsavedChanges(true);
	track_changed_since_last_update = true;
}

void GPSTrackRecorder::positionUpdatesInterrupted()
{
	target_template->getTrack().finishCurrentSegment();
	target_template->setHasUnsavedChanges(true);
	track_changed_since_last_update = true;
}

void GPSTrackRecorder::templateDeleted(int pos, const Template* old_temp)
{
	Q_UNUSED(pos);
	if (!is_active)
		return;
	
	if (old_temp == target_template)
	{
		// Deactivate
		gps_display->disconnect(this);
		draw_update_timer.stop();
		is_active = false;
	}
}

void GPSTrackRecorder::drawUpdate()
{
	if (!is_active)
		return;
	
	if (track_changed_since_last_update)
	{
		if (widget->getMapView()->isTemplateVisible(target_template))
			target_template->setTemplateAreaDirty();
		
		track_changed_since_last_update = false;
	}
}


}  // namespace LibreMapper
