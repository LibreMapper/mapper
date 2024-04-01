/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2015-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_OBJECT_SELECTOR_H
#define LIBREMAPPER_OBJECT_SELECTOR_H

#include <utility>
#include <vector>

#include <QtGlobal>

namespace LibreMapper {

class Map;
class MapCoordF;
class Object;

using SelectionInfoVector = std::vector<std::pair<int, Object*>>;


/**
 * Implements the object selection logic for edit tools.
 */
class ObjectSelector
{
public:
	/** Creates a selector for the given map. */
	ObjectSelector(Map* map);
	
	/**
	 * Selects an object at the given position.
	 * If there is already an object selected at this position, switches through
	 * the available objects.
	 * @param tolerance maximum, normal selection distance in map units.
	 *    It is enlarged by 1.5 if no objects are found with the normal distance.
	 * @param toggle corresponds to the shift key modifier.
	 * @return true if the selection has changed.
	 */
	bool selectAt(const MapCoordF& position, qreal tolerance, bool toggle);
	
	/**
	 * Applies box selection.
	 * @param toggle corresponds to the shift key modifier.
	 * @return true if the selection has changed.
	 */
	bool selectBox(const MapCoordF& corner1, const MapCoordF& corner2, bool toggle);
	
	// TODO: move to other place? util.h/cpp or object.h/cpp
	static bool sortObjects(const std::pair<int, Object*>& a, const std::pair<int, Object*>& b);
	
private:
	bool selectionInfosEqual(const SelectionInfoVector& a, const SelectionInfoVector& b);
	
	// Information about the last click
	SelectionInfoVector last_results;
	SelectionInfoVector last_results_ordered;
	SelectionInfoVector::size_type next_object_to_select;
	
	Map* map;
};


}  // namespace LibreMapper

#endif
