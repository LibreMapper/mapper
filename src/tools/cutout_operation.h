/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2013, 2014, 2017-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_CUTOUT_OPERATION_H
#define LIBREMAPPER_CUTOUT_OPERATION_H

#include <vector>

#include "core/objects/boolean_tool.h"

namespace OpenOrienteering {

class AddObjectsUndoStep;
class Map;
class Object;
class PathObject;
class UndoStep;


/**
 * Operation to make map cutouts.
 * 
 * This functor must not be applied to map parts other than the current one.
 * 
 * See CutoutTool::apply for usage example.
 */
class CutoutOperation
{
public:
	/**
	 * Function object constructor.
	 * 
	 * Setting cut_away to true inverts the tool's effect, i.e. it will remove
	 * the object parts inside the outline of cutout_object instead of the
	 * object parts outside.
	 */
	CutoutOperation(Map* map, PathObject* cutout_object, bool cut_away);
	
	/** 
	 * This functor must not be copied.
	 * 
	 * If it is to be used via std::function, std::ref can be used to explicitly
	 * create a copyable reference.
	 */
	CutoutOperation(const CutoutOperation&) = delete;
	
	/**
	 * Destructs the object.
	 * 
	 * commit() must be called before.
	 */
	~CutoutOperation();
	
	/** 
	 * This functor must not be assigned to.
	 */
	CutoutOperation& operator=(const CutoutOperation&) = delete;
	
	/**
	 * Applies the configured cutting operation on the given object.
	 */
	void operator()(Object* object);
	
	/**
	 * Commits the changes.
	 * 
	 * This must always be called before the destructor.
	 * No other operations may be called after commit.
	 */
	void commit();
	
private:
	UndoStep* finish();
	
	Map* map;
	PathObject* cutout_object;
	std::vector<PathObject*> new_objects;
	AddObjectsUndoStep* add_step;
	BooleanTool boolean_tool;
	BooleanTool::PathObjects out_objects;
	bool cut_away;
};


}  // namespace OpenOrienteering

#endif
