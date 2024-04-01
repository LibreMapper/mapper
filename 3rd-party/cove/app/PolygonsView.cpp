/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 * Copyright 2020 Kai Pastor
 *
 * This file is part of CoVe.
 */

#include "PolygonsView.h"

#include <utility>

#include <Qt>
#include <QBrush>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QSize>
#include <QSizeF>
#include <QWidget>

#include "libvectorizer/Polygons.h"

#include "ImageView.h"

class QPaintEvent;

namespace cove {
//@{
//! \ingroup gui

/*! \class PolyImageWidget
 * \brief ImageWidget drawing \a PolygonList above the image.
 *
 * Helper class for PolygonsView.
 */

PolyImageWidget::PolyImageWidget(QWidget* parent)
	: ImageWidget(parent)
{
}

PolyImageWidget::~PolyImageWidget() = default;

/*! Return currently set PolygonList.  \sa setPolygons(PolygonList& p)
  */
const PolygonList& PolyImageWidget::polygons() const
{
	return polygonsList;
}

/*! What PolygonList to draw.
 * \bug Behaves differently from setImage().  Creates its own copy of
 * PolygonList and polygons() then returns another copy.
 */
void PolyImageWidget::setPolygons(PolygonList p)
{
	polygonsList = std::move(p);
	update();
}

/*! paintEvent called by Qt engine, calls inherited ImageWidget::paintEvent and
 * then draws lines. */
void PolyImageWidget::paintEvent(QPaintEvent* pe)
{
	ImageWidget::paintEvent(pe);
	if (!realPaintEnabled()) return;

	if (polygonsList.empty()) return;

	auto const pen = QPen(Qt::cyan);
	auto const brush = QBrush(Qt::red);
	auto const marker = QRectF(QPointF(-1.5, -1.5) / magnification(),
	                           QSizeF(3.0, 3.0) / magnification());

	auto const event_rect = pe->rect();
	auto const event_rectf = QRectF(
	                             event_rect.left() / magnification() + marker.left(),
	                             event_rect.top() / magnification() + marker.top(),
	                             event_rect.width() / magnification() + marker.width(),
	                             event_rect.height() / magnification() + marker.height());

	QPainter p(this);
	p.translate(magnification() / 2, magnification() / 2);  // offset for aliased painting
	p.scale(magnification(), magnification());

	for (auto const& polygon : polygonsList)
	{
		// when polygon does not interfere with current repainted area, skip it
		if (!event_rectf.intersects(polygon.boundingRect()))
			continue;

		// draw line
		p.setPen(pen);
		p.setBrush(Qt::NoBrush);
		if (polygon.isClosed())
			p.drawPolygon(polygon.data(), int(polygon.size()));
		else
			p.drawPolyline(polygon.data(), int(polygon.size()));

		// draw squares
		p.setPen(Qt::NoPen);
		p.setBrush(brush);
		for (auto const& pt : polygon)
		{
			if (event_rectf.contains(pt))
				p.drawRect(QRectF(pt + marker.topLeft(), marker.size()));
		}
	}
}

/*! \class PolygonsView
 * \brief Provides scrollable view of an QImage with vectors over it.
 */

/*! Default constructor.
 */
PolygonsView::PolygonsView(QWidget* parent)
	: ImageView(new PolyImageWidget(), parent)
{}

PolygonsView::~PolygonsView() = default;

/*! Gets the polygonList that is drawn over the image.
 * \sa setPolygons(PolygonList* p)
 */
const PolygonList& PolygonsView::polygons() const
{
	return qobject_cast<const PolyImageWidget*>(widget())->polygons();
}

/*! Sets the polygonList that is drawn over the image.
 */
void PolygonsView::setPolygons(PolygonList p)
{
	qobject_cast<PolyImageWidget*>(widget())->setPolygons(std::move(p));
}


}  // namespace cove

//@}
