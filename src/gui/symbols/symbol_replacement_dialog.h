/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2017-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_SYMBOL_REPLACEMENT_DIALOG_H
#define OPENORIENTEERING_SYMBOL_REPLACEMENT_DIALOG_H

#include <memory>
#include <vector>

#include <QObject>
#include <QDialog>
#include <QString>

#include "core/objects/symbol_rule_set.h"

class QCheckBox;
class QComboBox;
class QTableWidget;
class QWidget;

namespace OpenOrienteering {

class Map;
class SymbolDropDownDelegate;


/**
 * Dialog for assigning replacement symbols for original symbols and patterns.
 */
class SymbolReplacementDialog : public QDialog
{
	Q_OBJECT
	
public:
	enum Mode
	{
		ReplaceSymbolSet,  ///< Replace all current current symbols
		AssignByPattern,   ///< Assign new symbols based on patterns
	};
	
	SymbolReplacementDialog(QWidget* parent, Map& object_map, const Map& symbol_set, SymbolRuleSet& symbol_rules, Mode mode);
	~SymbolReplacementDialog() override;
	
	SymbolReplacementDialog(const SymbolReplacementDialog&) = delete;
	SymbolReplacementDialog(SymbolReplacementDialog&&) = delete;
	
	SymbolReplacementDialog& operator=(const SymbolReplacementDialog&) = delete;
	SymbolReplacementDialog&& operator=(SymbolReplacementDialog&&) = delete;
	
	SymbolRuleSet::Options replacementOptions() const;
	QString replacementId() const;
	
protected:
	void showHelp();
	
	void matchByName();
	void matchByNumber();
	void resetReplacements();
	
	void openCrtFile();
	bool saveCrtFile();
	
	void done(int r) override;
	
	void updateMappingTable();
	void updateMappingFromTable();
	
private:
	Map& object_map;
	const Map& symbol_set;
	SymbolRuleSet& symbol_rules;
	
	QCheckBox* import_all_check = nullptr;
	QCheckBox* delete_unused_symbols_check = nullptr;
	QCheckBox* delete_unused_colors_check = nullptr;
	QCheckBox* preserve_symbol_states_check = nullptr;
	QComboBox* id_edit;
	QTableWidget* mapping_table = nullptr;
	std::vector<std::unique_ptr<SymbolDropDownDelegate>> symbol_widget_delegates;
	
	Mode mode;
};


}  // namespace OpenOrienteering

#endif
