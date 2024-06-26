/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2016, 2018 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_GPS_DISPLAY_H
#define LIBREMAPPER_GPS_DISPLAY_H

#include <QtGlobal>
#include <QObject>
#include <QString>

#include "core/map_coord.h"

class QGeoPositionInfo;
class QGeoPositionInfoSource;
class QPainter;
class QTimerEvent;

namespace LibreMapper {

class Georeferencing;
class MapWidget;


/**
 * Retrieves the GPS position and displays a marker at this position on a MapWidget.
 * 
 * \todo Use qreal instead of float (in all sensor code) for consistency with Qt.
 */
class GPSDisplay : public QObject
{
Q_OBJECT
public:
	/// Creates a GPS display for the given map widget and georeferencing.
	GPSDisplay(MapWidget* widget, const Georeferencing& georeferencing, QObject* parent = nullptr);
	/// Destructor, removes the GPS display from the map widget.
	~GPSDisplay() override;
	
	/**
	 * Checks if GPS is enabled, and may guide the user to the device settings.
	 * 
	 * If GPS is not enabled in the device settings, it asks the user whether he
	 * wishes to open the device's location settings dialog.
	 * (At the moment, this is implemented for Android only.)
	 * 
	 * Returns true if GPS is enabled, but also when the settings dialog remains
	 * open when returning from this function and the final setting is unknown.
	 */
	bool checkGPSEnabled();
	
	/// Starts regular position updates. This will issue redraws of the map widget.
	void startUpdates();
	/// Stops regular position updates.
	void stopUpdates();
	
	/// Sets GPS marker visibility (true by default)
	void setVisible(bool visible);
	/// Returns GPS marker visibility
	bool isVisible() const { return visible; }
	
	/// Sets whether distance rings are drawn
	void enableDistanceRings(bool enable);
	/// Sets whether the current heading from the Compass is used to draw a heading indicator.
	void enableHeadingIndicator(bool enable);
	
	/// This is called from the MapWidget drawing code to draw the GPS position marker.
	void paint(QPainter* painter);
	
	/// Returns if a valid position was received since the last call to startUpdates().
	bool hasValidPosition() const { return has_valid_position; }
	/// Returns the latest received GPS coord. Check hasValidPosition() beforehand!
	const MapCoordF& getLatestGPSCoord() const { return latest_gps_coord; }
	/// Returns the accuracy of the latest received GPS coord, or -1 if unknown. Check hasValidPosition() beforehand!
	float getLatestGPSCoordAccuracy() const { return latest_gps_coord_accuracy; }
	
	/// Starts quick blinking for one or more seconds.
	void startBlinking(int seconds);
	
	/// Stops blinking.
	void stopBlinking();
	
	/// Returns true while blinking is active.
	bool isBlinking() const { return blink_count > 0; }
	
protected:
	/// Handles blinking.
	void timerEvent(QTimerEvent* e) override;
	
signals:
	/// Is emitted whenever a new position update happens.
	/// If the accuracy is unknown, -1 will be given.
	void mapPositionUpdated(const LibreMapper::MapCoordF& coord, float accuracy);
	
	/// Like mapPositionUpdated(), but gives the values as
	/// latitude / longitude in degrees and also gives altitude
	/// (meters above sea level; -9999 is unknown)
	void latLonUpdated(double latitude, double longitude, double altitude, float accuracy);
	
	/// Is emitted when updates are interrupted after previously being active,
	/// due to loss of satellite reception or another error such as the user
	/// turning off the GPS receiver.
	void positionUpdatesInterrupted();
	
private slots:
    void positionUpdated(const QGeoPositionInfo& info);
	void error();
	
private:
	MapCoordF calcLatestGPSCoord(bool& ok);
	void updateMapWidget();
	
	/**
	 * A lightweight utility for sinusoidal pulsating opacity.
	 * 
	 * This class depends on another object's QObject::timerEvent() override
	 * to advance the pulsation state and eventually stop the activity.
	 * 
	 * \see QBasicTimer
	 */
	class PulsatingOpacity
	{
	public:
		/// Returns true when the pulsation is active.
		bool isActive() const { return bool(timer_id); }
		/// Starts a timer on the given object.
		void start(QObject& object);
		/// Stops the timer on the given object.
		void stop(QObject& object);
		/// Returns the ID of the active timer.
		int timerId() const { return timer_id; }
		
		/// Advances the pulsation state.
		bool advance();
		/// Returns the current opacity.
		qreal current() const;
		
	private:
		int timer_id = 0;
		quint8 index = 0;
	};
	
	MapWidget* widget;
	const Georeferencing& georeferencing;
	QGeoPositionInfoSource* source = nullptr;
	MapCoordF latest_gps_coord;
	float latest_gps_coord_accuracy = 0;
	PulsatingOpacity pulsating_opacity;
	int blink_count = 0;
	bool tracking_lost             = false;
	bool has_valid_position        = false;
	bool gps_updated               = false;
	bool visible                   = false;
	bool distance_rings_enabled    = false;
	bool heading_indicator_enabled = false;
};


}  // namespace LibreMapper

#endif
