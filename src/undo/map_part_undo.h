/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_MAP_PART_UNDO_H
#define LIBREMAPPER_MAP_PART_UNDO_H

#include <QString>

#include "core/symbols/symbol.h"
#include "undo/undo.h"

class QXmlStreamReader;
class QXmlStreamWriter;

namespace OpenOrienteering {

class Map;
class MapPart;


/**
 * @brief A map part undo step handles addition, deletion and renaming of map parts.
 */
class MapPartUndoStep : public UndoStep
{
public:
	/**
	 * Types of map part changes.
	 */
	enum MapPartChange
	{
		AddMapPart      = 1,
		RemoveMapPart   = 2,
		ModifyMapPart   = 3,
		UndefinedChange = 0
	};
	
	/**
	 * Constructs a particular undo step for the given part.
	 */
	MapPartUndoStep(Map* map, MapPartChange change, const MapPart* part);
	
	/**
	 * Constructs a particular undo step for the part specified by index.
	 */
	MapPartUndoStep(Map* map, MapPartChange change, int index);
	
	/**
	 * Constructs an incomplete undo step of type UndefinedChange.
	 * 
	 * This is to be used with loadImpl().
	 */
	MapPartUndoStep(Map* map);
	
	~MapPartUndoStep() override;
	
	/**
	 * Returns true unless this step's type is UndefinedChange.
	 */
	bool isValid() const override;
	
	UndoStep* undo() override;

	bool getModifiedParts(PartSet& out) const override;
	
	void getModifiedObjects(int part_index, ObjectSet& out) const override;
	
	
protected:
	void saveImpl(QXmlStreamWriter& xml) const override;
	
	void loadImpl(QXmlStreamReader& xml, SymbolDictionary& symbol_dict) override;
	
	MapPartChange change;
	
	int index;
	
	QString name;
};


}  // namespace OpenOrienteering

#endif // MAP_PART_UNDO_H
