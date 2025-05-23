/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2017, 2024 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "object_selector.h"

#include <algorithm>
#include <cstddef>
// IWYU pragma: no_include <map>
// IWYU pragma: no_include <memory>

#include <QRectF>

#include "core/map.h"
// IWYU pragma: no_include "core/map_part.h"
#include "core/objects/object.h"


namespace LibreMapper {

ObjectSelector::ObjectSelector(Map* map)
 : map(map)
{
	// nothing else
}


bool ObjectSelector::selectAt(const MapCoordF& position, qreal tolerance, bool toggle)
{
	bool selection_changed;
	
	const bool single_object_selected = map->getNumSelectedObjects() == 1;
	Object* single_selected_object = nullptr;
	if (single_object_selected)
		single_selected_object = *map->selectedObjectsBegin();
	
	// Clicked - get objects below cursor
	SelectionInfoVector objects;
	map->findObjectsAt(position, 0.001 * tolerance, false, false, false, false, objects);
	if (objects.empty())
		map->findObjectsAt(position, 0.0015 * tolerance, false, true, false, false, objects);
	
	// Selection logic, trying to select the most relevant object(s)
	if (!toggle || map->getNumSelectedObjects() == 0)
	{
		if (objects.empty())
		{
			// Clicked on empty space, deselect everything
			selection_changed = map->getNumSelectedObjects() > 0;
			last_results.clear();
			map->clearObjectSelection(true);
		}
		else if (!last_results.empty() && selectionInfosEqual(objects, last_results))
		{
			// If this result is the same as last time, select next object
			next_object_to_select = next_object_to_select % last_results_ordered.size();
			
			map->clearObjectSelection(false);
			map->addObjectToSelection(last_results_ordered[next_object_to_select].second, true);
			selection_changed = true;
			
			++next_object_to_select;
		}
		else
		{
			// Results different - select object with highest priority, if it is not the same as before
			last_results = objects;
			std::sort(objects.begin(), objects.end(), compareTypeAndExtent);
			last_results_ordered = std::move(objects);
			next_object_to_select = 1;
			
			map->clearObjectSelection(false);
			if (single_selected_object == last_results_ordered.begin()->second)
			{
				next_object_to_select = next_object_to_select % last_results_ordered.size();
				map->addObjectToSelection(last_results_ordered[next_object_to_select].second, true);
				++next_object_to_select;
			}
			else
				map->addObjectToSelection(last_results_ordered.begin()->second, true);
			
			selection_changed = true;
		}
	}
	else
	{
		// Shift held and something is already selected
		if (objects.empty())
		{
			// do nothing
			selection_changed = false;
		}
		else if (!last_results.empty() && selectionInfosEqual(objects, last_results))
		{
			// Toggle last selected object - must work the same way, regardless if other objects are selected or not
			next_object_to_select = next_object_to_select % last_results_ordered.size();
			
			if (map->toggleObjectSelection(last_results_ordered[next_object_to_select].second, true) == false)
				++next_object_to_select;	// only advance if object has been deselected
			selection_changed = true;
		}
		else
		{
			// Toggle selection of highest priority object
			last_results = objects;
			std::sort(objects.begin(), objects.end(), compareTypeAndExtent);
			last_results_ordered = std::move(objects);
			
			map->toggleObjectSelection(last_results_ordered.begin()->second, true);
			selection_changed = true;
		}
	}
	
	return selection_changed;
}


bool ObjectSelector::selectBox(const MapCoordF& corner1, const MapCoordF& corner2, bool toggle)
{
	bool selection_changed = false;
	
	std::vector<Object*> objects;
	map->findObjectsAtBox(corner1, corner2, false, false, objects);
	
	if (!toggle)
	{
		if (map->getNumSelectedObjects() > 0)
			selection_changed = true;
		map->clearObjectSelection(false);
	}
	
	const auto size = objects.size();
	for (std::size_t i = 0; i < size; ++i)
	{
		if (toggle)
			map->toggleObjectSelection(objects[i], i == size - 1);
		else
			map->addObjectToSelection(objects[i], i == size - 1);
			
		selection_changed = true;
	}
	
	return selection_changed;
}


// static
bool ObjectSelector::compareTypeAndExtent(const std::pair<int, Object*>& a, const std::pair<int, Object*>& b)
{
	if (a.first != b.first)
		return a.first < b.first;
	
	auto a_area = a.second->getExtent().width() * a.second->getExtent().height();
	auto b_area = b.second->getExtent().width() * b.second->getExtent().height();
	return a_area < b_area;
}


// static
bool ObjectSelector::selectionInfosEqual(const SelectionInfoVector& a, const SelectionInfoVector& b)
{
	return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}


}  // namespace LibreMapper
