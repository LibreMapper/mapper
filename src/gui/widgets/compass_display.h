/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_COMPASS_DISPLAY_H
#define LIBREMAPPER_COMPASS_DISPLAY_H

#include <QtGlobal>
#include <QObject>
#include <QSize>
#include <QTime>
#include <QWidget>

class QHideEvent;
class QPaintEvent;
class QShowEvent;

namespace LibreMapper {


/**
 * A widget which displays a compass.
 * 
 * By default, this widget is transparent (Qt::WA_NoSystemBackground), i.e. it
 * it does not draw a background. For proper background drawing, it shall not be
 * a child but a sibling of the background widget.
 */
class CompassDisplay : public QWidget
{
Q_OBJECT
public:
	/** 
	 * Creates a compass display.
	 */
	CompassDisplay(QWidget* parent = nullptr);
	
	/** 
	 * Destructor.
	 */
	~CompassDisplay() override;
	
	/** 
	 * Sets the compass direction, and updates the widget.
	 * 
	 * This does nothing unless at least 200 ms elapsed since the last change.
	 */
	void setAzimuth(float azimuth_deg);
	
	QSize sizeHint() const override;
	
protected:
	void showEvent(QShowEvent* event) override;
	
	void hideEvent(QHideEvent* event) override;
	
	void paintEvent(QPaintEvent* event) override;
	
	qreal azimuth;
	QTime last_update_time;
};


}  // namespace LibreMapper

#endif
