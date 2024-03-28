/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 * Copyright 2020 Kai Pastor
 *
 * This file is part of CoVe.
 */

#ifndef COVE_POLYGONSVIEW_H
#define COVE_POLYGONSVIEW_H

#include <QObject>
#include <QString>

#include "libvectorizer/Polygons.h"

#include "ImageView.h"

class QPaintEvent;
class QWidget;

namespace cove {

class PolyImageWidget : public ImageWidget
{
	Q_OBJECT

public:
	PolyImageWidget(QWidget* parent = nullptr);

	PolyImageWidget(const PolyImageWidget&) = delete;
	PolyImageWidget(PolyImageWidget&&) = delete;
	~PolyImageWidget() override;

	PolyImageWidget& operator=(const PolyImageWidget&) = delete;
	PolyImageWidget& operator=(PolyImageWidget&&) = delete;

	const PolygonList& polygons() const;
	void setPolygons(PolygonList p);

protected:
	void paintEvent(QPaintEvent* pe) override;

private:
	PolygonList polygonsList;
};


class PolygonsView : public ImageView
{
	Q_OBJECT

public:
	PolygonsView(QWidget* parent = nullptr);

	PolygonsView(const PolygonsView&) = delete;
	PolygonsView(PolygonsView&&) = delete;
	~PolygonsView() override;

	PolygonsView& operator=(const PolygonsView&) = delete;
	PolygonsView& operator=(PolygonsView&&) = delete;

	const PolygonList& polygons() const;
	void setPolygons(PolygonList p);
};


}  // namespace cove

#endif
