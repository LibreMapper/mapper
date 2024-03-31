/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_BOOLEAN_TOOL_H
#define LIBREMAPPER_BOOLEAN_TOOL_H

#include <vector>

// IWYU pragma: no_include <algorithm>

namespace LibreMapper {

class CombinedUndoStep;
class Map;
class PathObject;


/**
 * Wraps some helper functions for boolean operations.
 * 
 * The implementation of the boolean operation tools is based on the Clipper
 * library (aka libpolyclipping) (http://www.angusj.com/delphi/clipper.php).
 * 
 * Because Clipper does not support bezier curves, areas are clipped as
 * polygonal approximations, and after clipping we try to rebuild the curves.
 * 
 * @todo This class should get unit tests.
 */
class BooleanTool
{
public:
	/**
	 * A list of PathObject elements.
	 */
	typedef std::vector< PathObject* > PathObjects;
	
	/**
	 * Types of boolean operation.
	 */
	enum Operation
	{
		Union,
		Intersection,
		Difference,
		XOr,
		MergeHoles
	};
	
	/**
	 * Constructs a tool for the given operation and map.
	 * 
	 * map must not be nullptr (for some member functions).
	 */
	BooleanTool(Operation op, Map* map);
	
	/**
	 * Executes the operation on the selected objects in the map.
	 * 
	 * The first selected object is treated special and must be a path.
	 * 
	 * @return True on success, false on error
	 */
	bool execute();
	
	/**
	 * Executes the operation per symbol on the selected objects in the map.
	 * 
	 * Executes the operation independently for every group of path objects
	 * which have got the same symbol. Objects which are not of type
	 * Object::Path are ignored.
	 * 
	 * Errors during the operation are ignored, too. The original objects the
	 * operation failed for remain unchanged. The operation continues for other
	 * groups of objects.
	 * 
	 * @return True if the map was changed, false otherwise.
	 */
	bool executePerSymbol();
	
	/**
	 * Executes the operation on particular objects.
	 * 
	 * This function does not (actively) change the collection of objects in the map
	 * or the selection.
	 * 
	 * @param subject               The primary affected object.
	 * @param in_objects            All objects to operate on. Must contain subject.
	 * @param out_objects           The resulting collection of objects.
	 */
	bool executeForObjects(
	        const PathObject* subject,
	        const PathObjects& in_objects,
	        PathObjects& out_objects ) const;
	
	/**
	 * Executes the Intersection and Difference operation on the given line object.
	 * 
	 * @param area                  The primary object, here defining a boundary.
	 * @param line                  The line object to operate on.
	 * @param out_objects           The resulting collection of objects.
	 */
	void executeForLine(
	        const PathObject* area,
	        const PathObject* line,
	        PathObjects& out_objects ) const;
	
protected:	
	/**
	 * Executes the operation on particular objects, and provides undo steps.
	 * 
	 * This function changes the collection of objects in the map and the selection.
	 * 
	 * @param subject               The primary affected object.
	 * @param in_objects            All objects to operate on. Must contain subject.
	 * @param out_objects           The resulting collection of objects.
	 * @param undo_step             A combined undo step which will be filled with sub steps.
	 */
	bool executeForObjects(
	        const PathObject* subject,
	        const PathObjects& in_objects,
	        PathObjects& out_objects,
	        CombinedUndoStep& undo_step );
	
	Operation const op;
	Map* const map;
};


}  // namespace LibreMapper

#endif
