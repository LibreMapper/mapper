/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_COMBINED_SYMBOL_SETTINGS_H
#define OPENORIENTEERING_COMBINED_SYMBOL_SETTINGS_H

#include <vector>

#include <QObject>
#include <QString>

#include "gui/symbols/symbol_properties_widget.h"

class QSpinBox;

namespace OpenOrienteering {

class CombinedSymbol;
class Symbol;
class SymbolSettingDialog;


class CombinedSymbolSettings : public SymbolPropertiesWidget
{
Q_OBJECT
public:
	CombinedSymbolSettings(CombinedSymbol* symbol, SymbolSettingDialog* dialog);
	~CombinedSymbolSettings() override;
	
	void reset(Symbol* symbol) override;
	
	/**
	 * Updates the content and state of all input fields.
	 */
	void updateContents();
	
	
protected:
	void addRow(unsigned int index);
	void numberChanged(int value);
	void symbolChanged();
	void editButtonClicked();
	void showEditDialog(int index);
	
private:
	CombinedSymbol* symbol;
	
	QSpinBox* number_edit;
	struct Widgets;
	std::vector<Widgets> widgets;
};


}  // namespace OpenOrienteering

#endif
