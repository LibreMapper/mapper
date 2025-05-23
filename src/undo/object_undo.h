/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014, 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_MAP_UNDO_H
#define LIBREMAPPER_MAP_UNDO_H

#include <cstddef>
#include <map>
#include <utility>
#include <vector>

#include <QObject>

#include "core/objects/object.h"
#include "core/symbols/symbol.h"
#include "undo/undo.h"

class QXmlStreamReader;
class QXmlStreamWriter;

namespace LibreMapper {

class Map;
class XmlElementReader;
class XmlElementWriter;


/** 
 * Base class for undo steps which modify objects of a single map part.
 */
class ObjectModifyingUndoStep : public UndoStep
{
public:
	/**
	 * Creates an ObjectModifyingUndoStep for the given map and its current part.
	 */
	ObjectModifyingUndoStep(Type type, Map* map);
	
	/**
	 * Creates an ObjectModifyingUndoStep for the given map and part.
	 */
	ObjectModifyingUndoStep(Type type, Map* map, int part_index);
	
	/**
	 * Destructor.
	 */
	~ObjectModifyingUndoStep() override;
	
	
	/**
	 * Returns the index of the map part modified by this undo step.
	 */
	int getPartIndex() const;
	
	/**
	 * Set the index of the map part modified by this undo step.
	 * 
	 * This must not be called after object have been added.
	 */
	void setPartIndex(int part_index);
	
	
	/**
	 * Returns true if no objects are modified by this undo step.
	 */
	virtual bool isEmpty() const;
	
	/**
	 * Adds an object (by index) to this step.
	 * 
	 * The object must belong to the step's part.
	 */
	virtual void addObject(int index);
	
	
	/**
	 * Adds the step's modified part to the container provided by out.
	 * 
	 * @return True if there are objects modified by this undo step, false otherwise.
	 */
	bool getModifiedParts(PartSet& out) const override;
	
	/**
	 * Adds the list of the step's modified objects to the container provided by out.
	 * 
	 * Only adds objects when the given part_index matches this step's part index.
	 */
	void getModifiedObjects(int part_index, ObjectSet& out) const override;
	
	
protected:
	/**
	 * Saves undo properties to the xml stream.
	 * 
	 * Implementations in derived classes shall first call the parent class'
	 * implementation, and then start a new element for additional properties.
	 */
	void saveImpl(QXmlStreamWriter& xml) const override;
	
	/**
	 * Saves object details to the xml stream.
	 * 
	 * The default implemenentation does nothing.
	 */
	virtual void saveObject(XmlElementWriter& xml, int index) const;
	
	/**
	 * Loads undo properties from the xml stream.
	 * 
	 * Implementations in derived classes shall first check the element's name
	 * for one of their own elements, and otherwise call the parent class'
	 * implementation.
	 */
	void loadImpl(QXmlStreamReader& xml, SymbolDictionary& symbol_dict) override;
	
	/**
	 * Loads object details from the xml stream.
	 * 
	 * The default implemenentation does nothing.
	 */
	virtual void loadObject(XmlElementReader& xml, int index);
	
	
private:
	/**
	 * The index of the part all modified objects belong to.
	 */
	int part_index;
	
protected:
	/**
	 * A list of indices referring to objects in a map part.
	 */
	typedef std::vector<int> ObjectList;
	
	/**
	 * Indices of the existing objects that are modified by this step.
	 */
	ObjectList modified_objects;
};



/**
 * Base class for undo steps which add new objects to the map.
 * 
 * Takes care of correctly reacting to symbol changes in the map,
 * which are not covered by the undo / redo system.
 * 
 */
class ObjectCreatingUndoStep : public QObject, public ObjectModifyingUndoStep
{
Q_OBJECT
public:
	/**
	 * Constructor.
	 */
	ObjectCreatingUndoStep(Type type, Map* map);
	
	/**
	 * Destructor.
	 */
	~ObjectCreatingUndoStep() override;
	
	
	/**
	 * Returns true if the step can still be undone.
	 * 
	 * The value returned by this function is taken from the "valid" member variable.
	 */
	bool isValid() const override;
	
	/**
	 * Must not be called.
	 * 
	 * Use the two-parameter signatures instead of this one.
	 * 
	 * Does nothing in release builds. Aborts the program in non-release builds.
	 * Reimplemented from ObjectModifyingUndoStep::addObject()).
	 */
	void addObject(int index) override;
	
	/**
	 * Adds an object to the undo step with given index.
	 */
	void addObject(int existing_index, Object* object);
	
	/**
	 * Adds an object to the undo step with the index of the existing object.
	 */
	void addObject(Object* existing, Object* object);
	
	/**
	 * @copybrief ObjectModifyingUndoStep::getModifiedObjects
	 */
	void getModifiedObjects(int, ObjectSet&) const override;
	
	
public slots:
	/**
	 * Adapts the symbol pointers of objects referencing the changed symbol.
	 */
	virtual void symbolChanged(int pos, const LibreMapper::Symbol* new_symbol, const LibreMapper::Symbol* old_symbol);
	
	/**
	 * Invalidates the undo step if a contained object references the deleted symbol.
	 */
	virtual void symbolDeleted(int pos, const LibreMapper::Symbol* old_symbol);
	
protected:
	/**
	 * @copybrief UndoStep::saveImpl()
	 */
	void saveImpl(QXmlStreamWriter& xml) const override;
	
	/**
	 * @copybrief UndoStep::loadImpl()
	 */
	void loadImpl(QXmlStreamReader& xml, SymbolDictionary& symbol_dict) override;
	
	/**
	 * A list of object instance which are currently not part of the map.
	 */
	std::vector<Object*> objects;
	
	/**
	 * A flag indicating whether this step is still valid.
	 */
	bool valid;
};



/**
 * Map undo step which replaces all affected objects by another set of objects.
 */
class ReplaceObjectsUndoStep : public ObjectCreatingUndoStep
{
Q_OBJECT
public:
	ReplaceObjectsUndoStep(Map* map);
	
	~ReplaceObjectsUndoStep() override;
	
	UndoStep* undo() override;
	
private:
	bool undone;
};

/**
 * Map undo step which deletes the referenced objects.
 * 
 * Take care of correct application order when mixing with an add step to
 * make sure that the object indices are preserved.
 */
class DeleteObjectsUndoStep : public ObjectModifyingUndoStep
{
public:
	DeleteObjectsUndoStep(Map* map);
	
	~DeleteObjectsUndoStep() override;
	
	UndoStep* undo() override;
	
	bool getModifiedParts(PartSet& out) const override;
	
	void getModifiedObjects(int part_index, ObjectSet& out) const override;
};

/**
 * Map undo step which adds the contained objects.
 * 
 * Take care of correct application order when mixing with a delete step to
 * make sure that the object indices are preserved.
 */
class AddObjectsUndoStep : public ObjectCreatingUndoStep
{
Q_OBJECT
public:
	AddObjectsUndoStep(Map* map);
	
	~AddObjectsUndoStep() override;
	
	UndoStep* undo() override;
	
	/**
	 * Removes all contained objects from the map.
	 * 
	 * This can be useful after constructing the undo step,
	 * as it is often impractical to remove the objects directly
	 * when adding them as the indexing would be changed this way.
	 */
	void removeContainedObjects(bool emit_selection_changed);
	
protected:
	static bool sortOrder(const std::pair<int, int>& a, const std::pair<int, int>& b);
	
private:
	bool undone;
};



/**
 * Map undo step which assigns the referenced objects to another part.
 */
class SwitchPartUndoStep : public ObjectModifyingUndoStep
{
public:
	SwitchPartUndoStep(Map* map, int source_index, int target_index);
	
	SwitchPartUndoStep(Map* map);
	
	~SwitchPartUndoStep() override;
	
	bool getModifiedParts(PartSet& out) const override;
	
	void getModifiedObjects(int part_index, ObjectSet& out) const override;
	
	UndoStep* undo() override;
	
	
protected:
	void saveImpl(QXmlStreamWriter& xml) const override;
	
	void loadImpl(QXmlStreamReader& xml, SymbolDictionary& symbol_dict) override;
	
	int source_index;
	
	bool reverse = false;
};



/**
 * Map undo step which changes the symbols of referenced objects.
 */
class SwitchSymbolUndoStep : public QObject, public ObjectModifyingUndoStep
{
Q_OBJECT
public:
	SwitchSymbolUndoStep(Map* map);
	
	~SwitchSymbolUndoStep() override;
	
	bool isValid() const override;
	
	using ObjectModifyingUndoStep::addObject;
	virtual void addObject(int index, const Symbol* target_symbol);
	
	UndoStep* undo() override;
	
	
public slots:
	virtual void symbolChanged(int pos, const LibreMapper::Symbol* new_symbol, const LibreMapper::Symbol* old_symbol);
	
	virtual void symbolDeleted(int pos, const LibreMapper::Symbol* old_symbol);
	
protected:
	void saveImpl(QXmlStreamWriter& xml) const override;
	
	void loadImpl(QXmlStreamReader& xml, SymbolDictionary& symbol_dict) override;
	
	std::vector<const Symbol*> target_symbols;
	
	bool valid;
};



/**
 * Map undo step which reverses the directions of referenced objects,
 * 
 * thereby switching the side of any line mid symbols (often dashes of fences).
 */
class SwitchDashesUndoStep : public ObjectModifyingUndoStep
{
public:
	SwitchDashesUndoStep(Map* map);
	
	~SwitchDashesUndoStep() override;
	
	UndoStep* undo() override;
};



/**
 * Undo step which modifies object tags.
 */
class ObjectTagsUndoStep : public ObjectModifyingUndoStep
{
public:
	ObjectTagsUndoStep(Map* map);
	
	~ObjectTagsUndoStep() override;
	
	void addObject(int index) override;
	
	UndoStep* undo() override;
	
protected:
	void saveObject(XmlElementWriter& xml, int index) const override;
	
	void loadObject(XmlElementReader& xml, int index) override;
	
	typedef std::map<int, KeyValueContainer> ObjectTagsMap;
	
	ObjectTagsMap object_tags_map;
};


// ### ObjectModifyingUndoStep inline code ###

inline
int ObjectModifyingUndoStep::getPartIndex() const
{
	return part_index;
}


}  // namespace LibreMapper

#endif
