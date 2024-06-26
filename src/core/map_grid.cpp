/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "map_grid.h"

#include <QtMath>
#include <QPainter>
#include <QXmlStreamReader>

#include "core/georeferencing.h"
#include "core/map.h"
#include "core/map_coord.h"
#include "util/util.h"
#include "util/xml_stream_util.h"


namespace LibreMapper {

// ### MapGrid ###

MapGrid::MapGrid()
{
	snapping_enabled = true;
	color = qRgba(100, 100, 100, 128);
	display = AllLines;
	
	alignment = MagneticNorth;
	additional_rotation = 0;
	
	unit = MetersInTerrain;
	horz_spacing = 500;
	vert_spacing = 500;
	horz_offset = 0;
	vert_offset = 0;
}



void MapGrid::save(QXmlStreamWriter& xml) const
{
	XmlElementWriter element{xml, QLatin1String("grid")};
	const auto name_format = qAlpha(color) == 0xff ? QColor::HexRgb : QColor::HexArgb;
	element.writeAttribute(QLatin1String("color"), QColor::fromRgba(color).name(name_format));
	element.writeAttribute(QLatin1String("display"), display);
	element.writeAttribute(QLatin1String("alignment"), alignment);
	element.writeAttribute(QLatin1String("additional_rotation"), additional_rotation, 2);
	element.writeAttribute(QLatin1String("unit"), unit);
	element.writeAttribute(QLatin1String("h_spacing"), horz_spacing, 1);
	element.writeAttribute(QLatin1String("v_spacing"), vert_spacing, 1);
	element.writeAttribute(QLatin1String("h_offset"), horz_offset, 1);
	element.writeAttribute(QLatin1String("v_offset"), vert_offset, 1);
	element.writeAttribute(QLatin1String("snapping_enabled"), snapping_enabled);
}

const MapGrid& MapGrid::load(QXmlStreamReader& xml)
{
	Q_ASSERT(xml.name() == QLatin1String("grid"));
	
	XmlElementReader element(xml);
	QXmlStreamAttributes attributes = xml.attributes();
	color = QColor(element.attribute<QString>(QLatin1String("color"))).rgba();
	display = MapGrid::DisplayMode(element.attribute<int>(QLatin1String("display")));
	alignment = MapGrid::Alignment(element.attribute<int>(QLatin1String("alignment")));
	additional_rotation = element.attribute<double>(QLatin1String("additional_rotation"));
	unit = MapGrid::Unit(element.attribute<int>(QLatin1String("unit")));
	horz_spacing = element.attribute<double>(QLatin1String("h_spacing"));
	vert_spacing = element.attribute<double>(QLatin1String("v_spacing"));
	horz_offset = element.attribute<double>(QLatin1String("h_offset"));
	vert_offset = element.attribute<double>(QLatin1String("v_offset"));
	snapping_enabled = element.attribute<bool>(QLatin1String("snapping_enabled"));
	
	return *this;
}

void MapGrid::draw(QPainter* painter, const QRectF& bounding_box, Map* map, qreal scale_adjustment) const
{
	double final_horz_spacing, final_vert_spacing;
	double final_horz_offset, final_vert_offset;
	double final_rotation;
	calculateFinalParameters(final_horz_spacing, final_vert_spacing, final_horz_offset, final_vert_offset, final_rotation, map);
	
	QPen pen(color);
	if (qIsNull(scale_adjustment))
	{
		// zero-width cosmetic pen (effectively one pixel)
		pen.setWidth(0);
		pen.setCosmetic(true);
	}
	else
	{
		// 0.1 mm wide non-cosmetic pen
		pen.setWidthF(qreal(0.1) / scale_adjustment);
	}
	painter->setPen(pen);
	painter->setBrush(Qt::NoBrush);
	painter->setOpacity(qAlpha(color) / 255.0);
	
	auto draw_line = std::function<void (const QPointF&, const QPointF&)>{ [painter](const QPointF& p1, const QPointF& p2) {
		painter->drawLine(p1, p2);
	} };
	
	if (display == AllLines)
		Util::gridOperation(bounding_box, final_horz_spacing, final_vert_spacing, final_horz_offset, final_vert_offset, final_rotation, draw_line);
	else if (display == HorizontalLines)
		Util::hatchingOperation(bounding_box, final_vert_spacing, final_vert_offset, final_rotation - M_PI / 2, draw_line);
	else // if (display == VerticalLines)
		Util::hatchingOperation(bounding_box, final_horz_spacing, final_horz_offset, final_rotation, draw_line);
}

void MapGrid::calculateFinalParameters(double& final_horz_spacing, double& final_vert_spacing, double& final_horz_offset, double& final_vert_offset, double& final_rotation, Map* map) const
{
	double factor = ((unit == MetersInTerrain) ? (1000.0 / map->getScaleDenominator()) : 1);
	final_horz_spacing = factor * horz_spacing;
	final_vert_spacing = factor * vert_spacing;
	final_horz_offset = factor * horz_offset;
	final_vert_offset = factor * vert_offset;
	final_rotation = additional_rotation + M_PI / 2;
	
	const Georeferencing& georeferencing = map->getGeoreferencing();
	if (alignment == GridNorth)
	{
		final_rotation += georeferencing.getGrivation() * M_PI / 180;
		
		// Shift origin to projected coordinates origin
		double prev_horz_offset = MapCoordF::dotProduct(MapCoordF(0, -1).rotated(final_rotation), MapCoordF(georeferencing.getMapRefPoint().x(), -1 * georeferencing.getMapRefPoint().y()));
		double target_horz_offset = MapCoordF::dotProduct(MapCoordF(0, -1).rotated(additional_rotation + M_PI / 2), MapCoordF(georeferencing.getProjectedRefPoint().x(), georeferencing.getProjectedRefPoint().y()));
		final_horz_offset -= factor * target_horz_offset - prev_horz_offset;
		
		double prev_vert_offset = MapCoordF::dotProduct(MapCoordF(1, 0).rotated(final_rotation), MapCoordF(georeferencing.getMapRefPoint().x(), -1 * georeferencing.getMapRefPoint().y()));
		double target_vert_offset = MapCoordF::dotProduct(MapCoordF(1, 0).rotated(additional_rotation + M_PI / 2), MapCoordF(georeferencing.getProjectedRefPoint().x(), georeferencing.getProjectedRefPoint().y()));
		final_vert_offset += factor * target_vert_offset - prev_vert_offset;
	}
	else if (alignment == TrueNorth)
		final_rotation += georeferencing.getDeclination() * M_PI / 180;
}

MapCoordF MapGrid::getClosestPointOnGrid(MapCoordF position, Map* map) const
{
	double final_horz_spacing, final_vert_spacing;
	double final_horz_offset, final_vert_offset;
	double final_rotation;
	calculateFinalParameters(final_horz_spacing, final_vert_spacing, final_horz_offset, final_vert_offset, final_rotation, map);
	
	position.rotate(final_rotation - M_PI / 2);
	position.setX(qRound((position.x() - final_horz_offset) / final_horz_spacing) * final_horz_spacing + final_horz_offset);
	position.setY(qRound((position.y() - final_vert_offset) / final_vert_spacing) * final_vert_spacing + final_vert_offset);
	position.rotate(M_PI / 2 - final_rotation);
	return position;
}


bool MapGrid::hasAlpha() const
{
	return [](auto alpha) {
		return alpha > 0 && alpha < 255;
	}(qAlpha(color));
}



bool operator==(const MapGrid& lhs, const MapGrid& rhs)
{
	return
	  lhs.snapping_enabled == rhs.snapping_enabled &&
	  lhs.color            == rhs.color &&
	  lhs.display          == rhs.display &&
	  lhs.alignment        == rhs.alignment &&
	  lhs.additional_rotation == rhs.additional_rotation &&
	  lhs.unit             == rhs.unit &&
	  lhs.horz_spacing     == rhs.horz_spacing &&
	  lhs.vert_spacing     == rhs.vert_spacing &&
	  lhs.horz_offset      == rhs.horz_offset &&
	  lhs.vert_offset      == rhs.vert_offset;
}

bool operator!=(const MapGrid& lhs, const MapGrid& rhs)
{
	return !(lhs == rhs);
}


}  // namespace
