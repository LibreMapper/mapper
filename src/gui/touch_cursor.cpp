/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "touch_cursor.h"

#include <Qt>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QRectF>

#include "gui/map/map_widget.h"
#include "gui/util_gui.h"


namespace LibreMapper {

// TODO: convert these to settings
const float touch_pos_offset_mm = 25;
const float control_ring_radius_mm = 9.5f;

TouchCursor::TouchCursor(MapWidget* map_widget)
: visible(false)
, left_button_pressed(false)
, last_pressed_button(NoButton)
, map_widget(map_widget)
{
	// nothing
}

void TouchCursor::mousePressEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
		return;
	last_touch_pos = event->pos();
	first_move_event_received = false;
	
	ControlID control_id = NoButton;
	if (!visible || !touchedControl(event->pos(), &control_id))
	{
		// Jump to position
		updateMapWidget(false);
		
		QPoint cursor_pos = event->pos() - QPoint(0, touchPosOffsetPx());
		last_cursor_pos = cursor_pos;
		cursor_coord = map_widget->viewportToMapF(cursor_pos);
		visible = true;
		
		updateMapWidget(false);
		
		event->accept();
		auto synthetic_event = new QMouseEvent(
			QEvent::MouseMove, cursor_pos, event->globalPosition(),
			Qt::NoButton, event->buttons() & ~Qt::LeftButton, event->modifiers());
		QCoreApplication::sendEvent(qApp->instance(), synthetic_event);
		last_pressed_button = NoButton;
	}
	else if (control_id == LeftButton)
	{
		event->accept();
		auto synthetic_event = new QMouseEvent(
			QEvent::MouseButtonPress, map_widget->mapToViewport(cursor_coord),
			event->globalPosition(), event->button(), event->buttons(), event->modifiers());
		QCoreApplication::sendEvent(qApp->instance(), synthetic_event);		
		left_button_pressed = true;
		last_pressed_button = LeftButton;
		
		last_cursor_pos = map_widget->mapToViewport(cursor_coord);
	}
}

bool TouchCursor::mouseMoveEvent(QMouseEvent* event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return false;

	if (last_pressed_button != NoButton && !first_move_event_received)
	{
		first_move_event_received = true;
		last_touch_pos = event->pos();
		return false;
	}
	
	updateMapWidget(true);
	
	QPointF cursor_pos;
	if (last_pressed_button == LeftButton)
		cursor_pos = last_cursor_pos + (event->pos() - last_touch_pos);
	else
		cursor_pos = event->pos() - QPoint(0, touchPosOffsetPx());
	last_touch_pos = event->pos();
	last_cursor_pos = cursor_pos;
	cursor_coord = map_widget->viewportToMapF(cursor_pos);
		
	event->accept();
	auto synthetic_event = new QMouseEvent(
		QEvent::MouseMove, cursor_pos, event->globalPosition(),
		left_button_pressed ? event->button() : Qt::NoButton,
		left_button_pressed ? event->buttons() : (event->buttons() & ~Qt::LeftButton),
		event->modifiers());
	QCoreApplication::sendEvent(qApp->instance(), synthetic_event);		
	
	updateMapWidget(true);
	return true;
}

bool TouchCursor::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
		return true;
	
	if (left_button_pressed)
	{
		event->accept();
		auto synthetic_event = new QMouseEvent(
			QEvent::MouseButtonRelease, map_widget->mapToViewport(cursor_coord),
			event->globalPosition(), event->button(), event->buttons(), event->modifiers());
		QCoreApplication::sendEvent(qApp->instance(), synthetic_event);		
		left_button_pressed = false;
		return true;
	}
	else
		return false;
}

bool TouchCursor::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (last_pressed_button == LeftButton)
	{
		event->accept();
		auto synthetic_event = new QMouseEvent(
			QEvent::MouseButtonDblClick, map_widget->mapToViewport(cursor_coord),
			event->globalPosition(), event->button(), event->buttons(), event->modifiers());
		QCoreApplication::sendEvent(qApp->instance(), synthetic_event);		
		return true;
	}
	else
		return false;
}

void TouchCursor::paint(QPainter* painter)
{
	if (!visible)
		return;
	
	QPointF cursor_pos = map_widget->mapToViewport(cursor_coord);
	
	// Draw cursor
	QPixmap cursor_pixmap = map_widget->cursor().pixmap();
	if (!cursor_pixmap.isNull())
		painter->drawPixmap(cursor_pos - map_widget->cursor().hotSpot(), cursor_pixmap);
	else
	{
		// TODO: better standard "cursor"?
		float cursor_radius = standardCursorRadiusPx();
		
		painter->setPen(QPen(Qt::gray, controlRingStrokeRadiusPx()));
		painter->setBrush(Qt::NoBrush);
		painter->drawLine(cursor_pos - QPointF(cursor_radius, 0), cursor_pos + QPointF(cursor_radius, 0));
		painter->drawLine(cursor_pos - QPointF(0, cursor_radius), cursor_pos + QPointF(0, cursor_radius));
	}
	
	// Draw move handle / left button
	painter->setPen(QPen(Qt::gray, controlRingStrokeRadiusPx()));
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(cursor_pos + QPointF(0, touchPosOffsetPx()), controlRingRadiusPx(), controlRingRadiusPx());
}

bool TouchCursor::touchedControl(const QPoint& pos, TouchCursor::ControlID* out_id)
{
	QPointF cursor_pos = map_widget->mapToViewport(cursor_coord);
	QPointF control_ring_center = cursor_pos + QPointF(0, touchPosOffsetPx());
	
	QPointF dist_to_center = pos - control_ring_center;
	if (dist_to_center.x()*dist_to_center.x() + dist_to_center.y()*dist_to_center.y() < controlRingRadiusPx()*controlRingRadiusPx())
	{
		*out_id = LeftButton;
		return true;
	}
	
	return false;
}

void TouchCursor::updateMapWidget(bool delayed)
{
	QRectF fake_rect = QRectF(cursor_coord.x(), cursor_coord.y(), 0.0001f, 0.0001f);
	float pixel_border = qMax(touchPosOffsetPx() + controlRingRadiusPx(), standardCursorRadiusPx()) + controlRingStrokeRadiusPx() + 1;
	
	if (delayed)
		map_widget->updateDrawingLater(fake_rect, pixel_border);
	else
		map_widget->updateDrawing(fake_rect, pixel_border);
}

float TouchCursor::touchPosOffsetPx() const
{
	return Util::mmToPixelLogical(touch_pos_offset_mm);
}

float TouchCursor::controlRingRadiusPx() const
{
	return Util::mmToPixelLogical(control_ring_radius_mm);
}

float TouchCursor::controlRingStrokeRadiusPx() const
{
	return Util::mmToPixelLogical(0.5f);
}

float TouchCursor::standardCursorRadiusPx() const
{
	return Util::mmToPixelLogical(1.5f);
}


}  // namespace LibreMapper
