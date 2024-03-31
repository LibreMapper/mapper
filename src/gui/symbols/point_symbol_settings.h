/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_POINT_SYMBOL_SETTINGS_H
#define LIBREMAPPER_POINT_SYMBOL_SETTINGS_H

#include <QObject>
#include <QString>

#include "gui/symbols/symbol_properties_widget.h"

class QVBoxLayout;
class QWidget;

namespace LibreMapper {

class PointSymbol;
class PointSymbolEditorWidget;
class Symbol;
class SymbolSettingDialog;


class PointSymbolSettings : public SymbolPropertiesWidget
{
Q_OBJECT
public:
	PointSymbolSettings(PointSymbol* symbol, SymbolSettingDialog* dialog);
	~PointSymbolSettings() override;
	
	void reset(Symbol* symbol) override;
	
public slots:
	void tabChanged(int index);
	
private:
	PointSymbol* symbol;
	PointSymbolEditorWidget* symbol_editor;
	QVBoxLayout* layout;
	QWidget* point_tab;
	
};


}  // namespace LibreMapper

#endif
