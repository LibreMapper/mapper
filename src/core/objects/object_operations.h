/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schoeps (OpenOrienteering)
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_OBJECT_OPERATIONS_H
#define OPENORIENTEERING_OBJECT_OPERATIONS_H

#include "core/map.h"
#include "core/objects/object.h"
#include "core/symbols/symbol.h"

namespace OpenOrienteering {

/**
 * Object conditions and processors,
 * see methods Map::applyOnAllObjects() and MapPart::applyOnAllObjects()
 */
namespace ObjectOp
{
	// Conditions
	
	/** Returns true for objects with the given symbol. */
	struct HasSymbol
	{
		const Symbol* symbol;
		
		bool operator()(const Object* object) const noexcept
		{
			return object->getSymbol() == symbol;
		}
	};
	
	/** Returns true for objects with the given symbol type. */
	struct HasSymbolType
	{
		Symbol::Type type;
		
		bool operator()(const Object* object) const noexcept
		{
			return object->getSymbol()->getType() == type;
		}
	};
	
	/** Returns true for objects where the symbol type contains the given type. */
	struct ContainsSymbolType
	{
		Symbol::Type type;
		
		bool operator()(const Object* object) const noexcept
		{
			return object->getSymbol()->getContainedTypes() & type;
		}
	};
	
	
	// Operations
	
	/** Scales objects by the given factor. */
	struct Scale
	{
		double factor;
		MapCoordF center;
		
		void operator()(Object* object) const
		{
			object->scale(center, factor);
			object->update();
		}
	};
	
	/** Rotates objects by the given angle (in radians). */
	struct Rotate
	{
		double angle;
		MapCoordF center;
		
		void operator()(Object* object) const
		{
			object->rotateAround(center, angle);
			object->update();
		}
	};
	
	/**
	 * Changes the objects' symbols.
	 * NOTE: Make sure to apply this to correctly fitting objects only!
	 */
	struct ChangeSymbol
	{
		const Symbol* new_symbol;
		
		void operator()(Object* object, MapPart* part, int object_index) const
		{
			if (!object->setSymbol(new_symbol, false))
				part->deleteObject(object_index);
			else
				object->update();
		}
	};
	
	/** Delete objects. */
	struct Delete
	{
		void operator()(const Object*, MapPart* part, int object_index) const
		{
			part->deleteObject(object_index);
		}
	};
}


}  // namespace OpenOrienteering

#endif
