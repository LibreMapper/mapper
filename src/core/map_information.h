/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Kai Pastor (OpenOrienteering)
 * Copyright 2024 Matthias Kühlewein (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_MAP_INFORMATION_H
#define LIBREMAPPER_MAP_INFORMATION_H

#include <vector>

#include <QString>


namespace LibreMapper {

class Map;

class MapInformation
{
public:
	struct TreeItem {
		int level;              ///< Depth in the hierarchy
		QString label;          ///< Display label
		QString value = {};     ///< Auxiliary value
	};
	
	/**
	 * Constructs the map information object.
	 */
	explicit MapInformation(const Map* map);
	
	/**
	 * A sequence which defines a hierarchy of map information in text form.
	 */
	const std::vector<TreeItem>& treeItems() const { return tree_items; }
	
	/**
	 * Create a text report.
	 */
	QString makeTextReport(int indent = 2) const;
	
private:
	std::vector<TreeItem> tree_items;
};

}  // namespace LibreMapper

#endif // LIBREMAPPER_MAP_INFORMATION_H
