/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2018 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_ICON_PROPERTIES_WIDGET_H
#define LIBREMAPPER_ICON_PROPERTIES_WIDGET_H

#include <QWidget>

#include "gui/symbols/symbol_properties_widget.h"

class QAbstractButton;
class QLabel;
class QSpinBox;
class QLineEdit;

namespace LibreMapper {

class Map;
class Symbol;


/**
 * A widget for modifying symbol icons.
 */
class IconPropertiesWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit IconPropertiesWidget(Symbol* symbol, SymbolSettingDialog* dialog = nullptr);
	
	~IconPropertiesWidget() override;
	
	void reset(Symbol* symbol);
	
signals:
	void iconModified();
	
protected:
	void updateWidgets();
	
	void sizeEdited(int size);
	
	void copyClicked();
	
	void saveClicked();
	
	void loadClicked();
	
	void clearClicked();
	
private:
	SymbolSettingDialog* const dialog;
	Symbol* symbol;
	
	QLabel* default_icon_display;
	QSpinBox* default_icon_size_edit;
	QLabel* custom_icon_display;
	QLineEdit* custom_icon_size_edit;
	QAbstractButton* save_default_button;
	QAbstractButton* copy_default_button;
	QAbstractButton* save_button;
	QAbstractButton* load_button;
	QAbstractButton* clear_button;
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_ICON_PROPERTIES_WIDGET_H
