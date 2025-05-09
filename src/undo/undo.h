/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_UNDO_H
#define LIBREMAPPER_UNDO_H

#include "core/symbols/symbol.h"

#include <set>
#include <vector>

class QXmlStreamReader;
class QXmlStreamWriter;

namespace LibreMapper {

class Map;
class Object;


/**
 * Abstract base class for map editing undo steps.
 * 
 * UndoStep stores information which is necessary for executing an undo step.
 * While executing the step, creates a new UndoStep for the corresponding
 * redo step.
 * 
 * @see UndoManager
 */
class UndoStep
{
public:
	/**
	 * Types of undo steps for identification.
	 * 
	 * This is used by the file formats - do not change
	 * existing values.
	 */
	enum Type
	{
		ReplaceObjectsUndoStepType =   0,
		DeleteObjectsUndoStepType  =   1,
		AddObjectsUndoStepType     =   2,
		SwitchSymbolUndoStepType   =   3,
		SwitchDashesUndoStepType   =   4,
		CombinedUndoStepType       =   5,
		ValidNoOpUndoStepType      =   6,
		ObjectTagsUndoStepType     =   7,
		MapPartUndoStepType        =   8,
		SwitchPartUndoStepTypeV0   =   9,
		SwitchPartUndoStepType     =  10,
		InvalidUndoStepType        = 999
	};
	
	/**
	 * A set of integers referring to parts.
	 */
	typedef std::set<int> PartSet;
	
	
	/**
	 * A set of pointers to objects.
	 */
	typedef std::set<Object*> ObjectSet;
	
	
	/**
	 * Constructs an undo step of the given type.
	 */
	static UndoStep* getUndoStepForType(Type type, Map* map);
	
	
	/**
	 * Constructs an undo step having the given type.
	 */
	UndoStep(Type type, Map* map);
	
	UndoStep(const UndoStep&) = delete;
	UndoStep(UndoStep&&) = delete;
	
	/**
	 * Destructor.
	 */
	virtual ~UndoStep();
	
	
	UndoStep& operator=(const UndoStep&) = delete;
	UndoStep& operator=(UndoStep&&) = delete;
	
	
	/**
	 * Returns the type of the undo step.
	 */
	Type getType() const;
	
	
	/**
	 * Returns true if the step can still be undone.
	 * 
	 * Initially (after generating the step) this must return true.
	 * (However, this is different for InvalidUndoStep.)
	 * 
	 * Derived classes may return false to indicate that the step is no longer valid.
	 * This may happen after an object the step depends on,
	 * which is not tracked by the undo system, is deleted.
	 * 
	 * Example: changing a map object's symbol to a different one,
	 * then deleting the first symbol. Then changing the symbol cannot be undone
	 * as the old symbol does not exist anymore.
	 */
	virtual bool isValid() const;
	
	
	/**
	 * Undoes the action and returns a new UndoStep.
	 * 
	 * The returned UndoStep can redo the action again.
	 */
	virtual UndoStep* undo() = 0;
	
	
	/**
	 * Adds the list of the step's modified parts to the container provided by out.
	 * 
	 * The default implementation does nothing and returns false.
	 * 
	 * @return True if there are parts (and objects!) modified by this undo step, false otherwise.
	 */
	virtual bool getModifiedParts(PartSet& out) const;
	
	/**
	 * Adds the list of the step's modified objects to the container provided by out.
	 * 
	 * Only objects which belong to the given part are dealt with.
	 * 
	 * The default implementation does nothing.
	 */
	virtual void getModifiedObjects(int part_index, ObjectSet& out) const;
	
	
	/**
	 * Loads the undo step from the stream in xml format.
	 */
	static UndoStep* load(QXmlStreamReader& xml, Map* map, SymbolDictionary& symbol_dict);
	
	/**
	 * Saves the undo step to the stream in xml format.
	 * 
	 * This method is not to be overwritten.
	 * 
	 * @see saveImpl()
	 */
	void save(QXmlStreamWriter& xml) const;
	
protected:	
	/**
	 * Saves undo properties to the xml stream.
	 * 
	 * Implementations in derived classes shall first call the parent class'
	 * implementation, and then start a new element for additional properties.
	 */
	virtual void saveImpl(QXmlStreamWriter& xml) const;
	
	/**
	 * Loads undo properties from the xml stream.
	 * 
	 * Implementations in derived classes shall first check the element's name
	 * for one of their own elements, and otherwise call the parent class'
	 * implementation.
	 */
	virtual void loadImpl(QXmlStreamReader& xml, SymbolDictionary& symbol_dict);

protected:
	/**
	 * The type of the undo step.
	 */
	Type const type;
	
	/**
	 * The map this undo step belongs.
	 */
	Map* const map;
};



/**
 * @brief An undo step which is actually a sequence of sub UndoSteps.
 * 
 * A CombinedUndoStep bundles a sequence of one or more UndoSteps,
 * which it executes in reverse order.
 */
class CombinedUndoStep : public UndoStep
{
public:
	/**
	 * Constructs a composite undo step for the given map.
	 */
	CombinedUndoStep(Map* map);
	
	/**
	 * Destructor.
	 */
	~CombinedUndoStep() override;
	
	
	/**
	 * Returns true if all sub step can still be undone.
	 */
	bool isValid() const override;
	
	/**
	 * Undoes all sub steps in-order and returns a corresponding UndoStep.
	 */
	UndoStep* undo() override;
	
	
	/**
	 * Adds the modified parts of all sub steps to the given set.
	 */
	bool getModifiedParts(PartSet& out) const override;
	
	/**
	 * Adds the modified objects of all sub steps to the given set.
	 */
	void getModifiedObjects(int part_index, ObjectSet& out) const override;
	
	
	/** 
	 * Returns the number of sub steps.
	 */
	int getNumSubSteps() const;
	
	/** 
	 * Adds a sub step.
	 */
	void push(UndoStep* step);
	
	/** 
	 * Returns the i-th sub step.
	 */
	UndoStep* getSubStep(int i);
	
	
protected:
	/**
	 * @copybrief UndoStep::saveImpl()
	 */
	void saveImpl(QXmlStreamWriter& xml) const override;
	
	/**
	 * @copybrief UndoStep::loadImpl()
	 */
	void loadImpl(QXmlStreamReader& xml, SymbolDictionary& symbol_dict) override;
	
private:
	typedef std::vector<UndoStep*> StepList;
	
	StepList steps;
};



/**
 * An undo step which does nothing. 
 * 
 * Its validness depends on the argument given  to the constructor.
 * It always returns a valid NoOpUndoStep from undo().
 * 
 * This class is used to catch unknown types of undo step when loading files
 * from newer version of Mapper. Another use is unit-testing the UndoManager.
 */
class NoOpUndoStep : public UndoStep
{
public:
	/**
	 * Constructs an undo step for the given map, and determines its validness.
	 */
	NoOpUndoStep(Map* map, bool valid);
	
	/**
	 * Destructor.
	 */
	~NoOpUndoStep() override;
	
	
	/**
	 * Returns the validness as given to the constructor.
	 */
	bool isValid() const override;
	
	
	/**
	 * Returns a valid NoOpUndoStep.
	 * 
	 * Prints a warning if this undo step is not valid.
	 */
	UndoStep* undo() override;
	
	
private:
	bool const valid;
};



// ### UndoStep inline code ###

inline
UndoStep::Type UndoStep::getType() const
{
	return type;
}


// ### CombinedUndoStep inline code ###

inline
int CombinedUndoStep::getNumSubSteps() const
{
	return (int)steps.size();
}

inline
void CombinedUndoStep::push(UndoStep* step)
{
	steps.push_back(step);
}

inline
UndoStep* CombinedUndoStep::getSubStep(int i)
{
	return steps[i];
}


}  // namespace LibreMapper

#endif
