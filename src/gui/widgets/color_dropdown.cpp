/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2015, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "color_dropdown.h"

#include <Qt>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QStyle>
#include <QVariant>

#include "core/map.h"
#include "core/map_color.h"


namespace OpenOrienteering {

ColorDropDown::ColorDropDown(const Map* map, const MapColor* initial_color, bool spot_colors_only, QWidget* parent)
: QComboBox(parent)
, map(map)
, spot_colors_only(spot_colors_only)
{
	addItem(tr("- none -"), QVariant::fromValue<const MapColor*>(nullptr));
	
	int icon_size = style()->pixelMetric(QStyle::PM_SmallIconSize);
	QPixmap pixmap(icon_size, icon_size);
	
	int initial_index = 0;
	int num_colors = map->getNumColors();
	for (int i = 0; i < num_colors; ++i)
	{
		const MapColor* color = map->getColor(i);
		if (spot_colors_only && color->getSpotColorMethod() != MapColor::SpotColor)
			continue;
		
		if (initial_color == color)
			initial_index = count();
		
		pixmap.fill(colorWithOpacity(*color));
		QString name = spot_colors_only ? color->getSpotColorName() : map->translate(color->getName());
		addItem(QIcon(pixmap), name, QVariant::fromValue(color));
	}
	if (!spot_colors_only)
	{
		const int count = this->count();
		if (count > 0)
		{
			insertSeparator(count);
		}
		const MapColor* color = Map::getRegistrationColor();
		pixmap.fill(*color);
		addItem(QIcon(pixmap), color->getName(), QVariant::fromValue(color));
	}
	setCurrentIndex(initial_index);

	connect(map, &Map::colorAdded, this, &ColorDropDown::onColorAdded);
	connect(map, &Map::colorChanged, this, &ColorDropDown::onColorChanged);
	connect(map, &Map::colorDeleted, this, &ColorDropDown::onColorDeleted);
}


ColorDropDown::~ColorDropDown() = default;



const MapColor* ColorDropDown::color() const
{
	return itemData(currentIndex()).value<const MapColor*>();
}

void ColorDropDown::setColor(const MapColor* color)
{
	setCurrentIndex(findData(QVariant::fromValue(color)));
}

void ColorDropDown::addColor(const MapColor* color)
{
	if (!spot_colors_only || color->getSpotColorMethod() == MapColor::SpotColor)
	{
		int pos = 0;
		for (; pos < count(); ++pos)
		{
			const MapColor* c = itemData(pos).value<const MapColor*>();
			if (c && c->getPriority() > color->getPriority())
				break;
		}
		int icon_size = style()->pixelMetric(QStyle::PM_SmallIconSize);
		QPixmap pixmap(icon_size, icon_size);
		pixmap.fill(*color);
		insertItem(pos, map->translate(color->getName()), QVariant::fromValue(color));
		setItemData(pos, pixmap, Qt::DecorationRole);
	}
}

void ColorDropDown::updateColor(const MapColor* color)
{
	if (!spot_colors_only || color->getSpotColorMethod() == MapColor::SpotColor)
	{
		int pos = 0;
		for (; pos < count(); ++pos)
		{
			if (itemData(pos).value<const MapColor*>() == color)
				break;
		}
		
		if (pos < count())
		{
			int icon_size = style()->pixelMetric(QStyle::PM_SmallIconSize);
			QPixmap pixmap(icon_size, icon_size);
			pixmap.fill(*color);
			setItemText(pos, map->translate(color->getName()));
			setItemData(pos, pixmap, Qt::DecorationRole);
		}
		else
		{
			addColor(color);
		}
	}
	else
	{
		removeColor(color);
	}
}

void ColorDropDown::removeColor(const MapColor* color)
{
	for (int pos = 0; pos < count(); ++pos)
	{
		if (itemData(pos).value<const MapColor*>() == color)
		{
			if (currentIndex() == pos)
				setCurrentIndex(0);
			removeItem(pos);
			break;
		}
	}
}

void ColorDropDown::onColorAdded(int, const MapColor* color)
{
	addColor(color);
}

void ColorDropDown::onColorChanged(int, const MapColor* color)
{
	updateColor(color);
}

void ColorDropDown::onColorDeleted(int, const MapColor* color)
{
	removeColor(color);
}


}  // namespace OpenOrienteering
