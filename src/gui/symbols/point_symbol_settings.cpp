/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "point_symbol_settings.h"

#include <QtGlobal>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "core/symbols/point_symbol.h"
#include "core/symbols/symbol.h"
#include "gui/symbols/point_symbol_editor_widget.h"
#include "gui/symbols/symbol_setting_dialog.h"


namespace LibreMapper {

// ### PointSymbol ###

SymbolPropertiesWidget* PointSymbol::createPropertiesWidget(SymbolSettingDialog* dialog)
{
	return new PointSymbolSettings(this, dialog);
}



// ### PointSymbolSettings ###

PointSymbolSettings::PointSymbolSettings(PointSymbol* symbol, SymbolSettingDialog* dialog)
: SymbolPropertiesWidget(symbol, dialog), 
  symbol(symbol)
{
	symbol_editor = new PointSymbolEditorWidget(dialog->getPreviewController(), symbol, 0, true);
	connect(symbol_editor, &PointSymbolEditorWidget::symbolEdited, this, &SymbolPropertiesWidget::propertiesModified );
	
	layout = new QVBoxLayout();
	layout->addWidget(symbol_editor);
	
	point_tab = new QWidget();
	point_tab->setLayout(layout);
	addPropertiesGroup(tr("Point symbol"), point_tab);
	
	connect(this, &QTabWidget::currentChanged, this, &PointSymbolSettings::tabChanged);
}


PointSymbolSettings::~PointSymbolSettings() = default;



void PointSymbolSettings::reset(Symbol* symbol)
{
	if (Q_UNLIKELY(symbol->getType() != Symbol::Point))
	{
		qWarning("Not a point symbol: %s", symbol ? "nullptr" : qPrintable(symbol->getPlainTextName()));
		return;
	}
	
	SymbolPropertiesWidget::reset(symbol);
	this->symbol = static_cast<PointSymbol*>(symbol);
	
	layout->removeWidget(symbol_editor);
	delete(symbol_editor);
	
	symbol_editor = new PointSymbolEditorWidget(dialog->getPreviewController(), this->symbol, 0, true);
	connect(symbol_editor, &PointSymbolEditorWidget::symbolEdited, this, &SymbolPropertiesWidget::propertiesModified );
	layout->addWidget(symbol_editor);
}



void PointSymbolSettings::tabChanged(int /*index*/)
{
	symbol_editor->setEditorActive( currentWidget()==point_tab );
}


}  // namespace LibreMapper
