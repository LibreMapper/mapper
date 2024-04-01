/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "compass_display.h"

#include <QPainter>
#include <QtNumeric>

#include "gui/util_gui.h"


namespace LibreMapper {

CompassDisplay::CompassDisplay(QWidget* parent)
 : QWidget(parent)
 , azimuth(qSNaN())
 , last_update_time(QTime::currentTime())
{
	setAttribute(Qt::WA_NoSystemBackground, true);
}

CompassDisplay::~CompassDisplay()
{
	// nothing, not inlined
}

void CompassDisplay::setAzimuth(float azimuth_deg)
{
	constexpr int update_interval = 200;
	QTime current_time = QTime::currentTime();
	if (qAbs(last_update_time.msecsTo(current_time)) >= update_interval
	    && azimuth != azimuth_deg)
	{
		last_update_time = current_time;
		azimuth = azimuth_deg;
		update();
		if (auto parent = parentWidget())
			parent->update(geometry());
	}
}

QSize CompassDisplay::sizeHint() const
{
	auto width = qRound(Util::mmToPixelLogical(20.0));
	return QSize(width, width);
}

void CompassDisplay::showEvent(QShowEvent*)
{
	azimuth = qSNaN();
}

void CompassDisplay::hideEvent(QHideEvent*)
{
	// nothing
}

void CompassDisplay::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	
	auto margin = Util::mmToPixelLogical(1.0f);
	QRectF bounding_box = { {0,0}, size() };
	bounding_box.adjust(margin, margin, -margin, -margin);
	QPointF midpoint = bounding_box.center();
	QPointF endpoint = QPointF(midpoint.x(), bounding_box.top());
	QLineF line(midpoint, endpoint);
	
	auto have_value = !qIsNaN(azimuth);
	
	// Draw alignment cue
	constexpr qreal max_alignment_angle = 10;
	if (have_value && qAbs(azimuth) < max_alignment_angle)
	{
		float alignment_factor = (max_alignment_angle - qAbs(azimuth)) / max_alignment_angle;
		float radius = alignment_factor * 0.25f * bounding_box.height();
		painter.setPen(Qt::NoPen);
		painter.setBrush(Qt::green);
		painter.drawEllipse(line.pointAt(0.5f), radius, radius);
	}
	
	// Draw up marker
	float line_width = Util::mmToPixelLogical(0.3f);
	
	painter.setPen(QPen(Qt::white, line_width));
	painter.setBrush(Qt::NoBrush);
	painter.drawLine(line);
	
	painter.setPen(QPen(Qt::black, line_width));
	painter.drawLine(midpoint + QPointF(line_width, 0), endpoint + QPointF(line_width, 0));
	painter.drawLine(midpoint + QPointF(-1 * line_width, 0), endpoint + QPointF(-1 * line_width, 0));
	
	// Draw needle
	if (have_value)
	{
		painter.setPen(QPen(Qt::red, line_width));
		line.setAngle(90 + azimuth);
		painter.drawLine(line);
	}
}


}  // namespace LibreMapper
