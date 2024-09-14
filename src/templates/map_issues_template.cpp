/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */

#include "map_issues_template.h"

#include <map>
#include <memory>
#include <vector>

#include <QBrush>
#include <QColor>
#include <QLatin1String>
#include <QLineF>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QTransform>
#include <Qt>
#include <QtAssert>
#include <QtPreprocessorSupport>

#include "core/map.h"
#include "core/map_coord.h"
#include "core/map_issue_registry.h"
#include "core/objects/object.h"
#include "core/symbols/symbol.h"

namespace LibreMapper {

MapIssuesTemplate::MapIssuesTemplate(Map* map)
    : Template(QLatin1String{"- Map Issues -"}, map)
    , map(map)
{
	template_state = Loaded;
	connect(map, &Map::selectedObjectEdited, this, &MapIssuesTemplate::mapObjectsEdited);
}

MapIssuesTemplate::~MapIssuesTemplate()
{
	template_state = Unloaded;
}

void MapIssuesTemplate::drawTemplate(QPainter* painter, const QRectF& clip_rect, double scale, bool on_screen, qreal opacity) const
{
	Q_UNUSED(scale);
	Q_ASSERT(map == this->map);

	if (!on_screen)
		return;

	painter->save();
	painter->setOpacity(opacity);

	auto const registered_issues = map->getMapIssueRegistry()->getIssuesList();

	for (auto const& record : registered_issues)
	{
		auto const* object = record.object;
		if (!clip_rect.intersects(object->getExtent()))
			continue;

		auto highlight_pen = QPen { QColor(Qt::red) };
		/// \fixme Use scale instead of the world transform thingie.
		auto unit_length = painter->worldTransform().map(QLineF(0, 0, 1, 0)).length();
		highlight_pen.setWidthF(2.0 / unit_length);
		painter->setPen(highlight_pen);
		painter->setBrush(Qt::NoBrush);

		switch (object->getType())
		{
		case Object::Type::Point:
		case Object::Type::Text:
			painter->drawRect(object->getExtent());
			break;

		case Object::Type::Path:
			{
				auto const* path = object->asPath();

				QPainterPath shape {};
				auto move_on_next = true;
				auto const coord_count = path->getCoordinateCount();
				for (auto i = 0U; i < coord_count; ++i)
				{
					auto node = path->getCoordinate(i);
					if (move_on_next)
						shape.moveTo(QPointF(node));

					move_on_next = node.isHolePoint();

					if (node.isCurveStart())
					{
						shape.cubicTo(QPointF(path->getCoordinate(i+1)),
						              QPointF(path->getCoordinate(i+2)),
						              QPointF(path->getCoordinate(i+3)));
						i += 2;
					}
					else
					{
						shape.lineTo(QPointF(node));
					}
				}

				painter->drawPath(shape);

				if (path->getSymbol()->getContainedTypes().testFlag(Symbol::Area))
				{
					painter->setBrush(QBrush { Qt::red, Qt::DiagCrossPattern });
					painter->setPen(Qt::NoPen);
					painter->drawPath(shape);
				}

			}
			break;
		}
	}

	painter->restore();
}

Template* MapIssuesTemplate::duplicate() const
{
	auto* new_templ = new MapIssuesTemplate(map);
	return new_templ;
}

const char* MapIssuesTemplate::getTemplateType() const
{
	return "MapIssues";
}

bool MapIssuesTemplate::fileExists() const
{
	return true;
}

bool MapIssuesTemplate::isRasterGraphics() const
{
	return false;
}

bool MapIssuesTemplate::loadTemplateFileImpl()
{
	return true;
}

void MapIssuesTemplate::unloadTemplateFileImpl()
{
	// nothing
}

QRectF MapIssuesTemplate::calculateTemplateBoundingBox() const
{
	QRectF ret;

	auto const registered_issues = map->getMapIssueRegistry()->getIssuesList();

	for (auto const& record : registered_issues)
	{
		auto const* object = record.object;
		ret = ret.united(object->getExtent());
	}

	return ret;
}

void MapIssuesTemplate::saveTemplateConfiguration(QXmlStreamWriter& xml, bool open, const QDir* map_dir) const
{
	Q_UNUSED(xml);
	Q_UNUSED(open);
	Q_UNUSED(map_dir);

	// Intentionally left blank. We don't want to save this pseudo-template config into the file.
}

void MapIssuesTemplate::ensureVisible()
{
	if (our_template_pos >= 0 && map->getTemplate(our_template_pos) == this)
		return;

	our_template_pos = -1;

	for (auto i = 0; i < map->getNumTemplates(); ++i)
	{
		if (map->getTemplate(i) == this)
			our_template_pos = i;
	}

	if (our_template_pos < 0)
	{
		our_template_pos = map->getFirstFrontTemplate();
		map->addTemplate(our_template_pos, std::unique_ptr<MapIssuesTemplate>(this));
	}
}

void MapIssuesTemplate::mapObjectsEdited()
{
	auto const registered_issues = map->getMapIssueRegistry()->getIssuesList(); /// \fixme Wasteful iteration through a vector further down.

	bool update_view = false;

	for (auto const* object : map->selectedObjects())
		for (auto const& issue : registered_issues)
			if (issue.object == object)
			{
				update_view = true;
				break;
			}

	if (update_view)
		map->updateAllMapWidgets(); /// \fixme Is there a nicer way how to tell the map widgets to refresh the template cache?
}

}  // namespace LibreMapper
