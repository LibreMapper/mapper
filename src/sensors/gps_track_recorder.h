/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Thomas Schöps (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_GPS_TRACK_RECORDER_H
#define LIBREMAPPER_GPS_TRACK_RECORDER_H

#include <QObject>
#include <QString>
#include <QTimer>

namespace LibreMapper {

class MapWidget;
class Template;
class TemplateTrack;
class GPSDisplay;


/** Records GPS tracks into a TemplateTrack. */
class GPSTrackRecorder : public QObject
{
Q_OBJECT
public:
	GPSTrackRecorder(GPSDisplay* gps_display, TemplateTrack* target_template, int draw_update_interval_milliseconds = -1, MapWidget* widget = nullptr);

public slots:
	void newPosition(double latitude, double longitude, double altitude, float accuracy);
	void positionUpdatesInterrupted();
	void templateDeleted(int pos, const LibreMapper::Template* old_temp);
	void drawUpdate();
	
private:
	GPSDisplay* gps_display;
	TemplateTrack* target_template;
	MapWidget* widget;
	QTimer draw_update_timer;
	bool track_changed_since_last_update;
	bool is_active;
};


}  // namespace LibreMapper

#endif
