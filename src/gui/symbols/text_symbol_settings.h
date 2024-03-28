/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_TEXT_SYMBOL_SETTINGS_H
#define OPENORIENTEERING_TEXT_SYMBOL_SETTINGS_H

#include <QtGlobal>
#include <QObject>
#include <QString>

#include "gui/symbols/symbol_properties_widget.h"

class QCheckBox;
class QDoubleSpinBox;
class QFont;
class QFontComboBox;
class QLineEdit;
class QListWidget;
class QPushButton;
class QRadioButton;
class QWidget;

namespace OpenOrienteering {

class ColorDropDown;
class Symbol;
class SymbolSettingDialog;
class TextSymbol;


class TextSymbolSettings : public SymbolPropertiesWidget
{
Q_OBJECT
public:
	TextSymbolSettings(TextSymbol* symbol, SymbolSettingDialog* dialog);
    ~TextSymbolSettings() override;
	
	void reset(Symbol* symbol) override;
	
	void updateGeneralContents();
	
	void updateFramingContents();
	
	void updateCompatibilityCheckEnabled();
	
	void updateCompatibilityContents();
	
protected slots:
	void orientedToNorthClicked(bool checked);
	void fontChanged(const QFont& font);
	void fontSizeChanged(double value);
	void letterSizeChanged();
	void colorChanged();
	void checkToggled(bool checked);
	void spacingChanged(double value);
	void iconTextEdited(const QString& text);
	
	void framingCheckClicked(bool checked);
	void framingColorChanged();
	void framingModeChanged();
	void framingSettingChanged();
	
	void ocadCompatibilityButtonClicked(bool checked);
	void lineBelowCheckClicked(bool checked);
	void lineBelowSettingChanged();
	void customTabRowChanged(int row);
	void addCustomTabClicked();
	void removeCustomTabClicked();
	
protected:
	void updateFontSizeEdit();
	void updateLetterSizeEdit();
	qreal calculateLetterHeight() const;
	
private:
	TextSymbol* symbol;
	SymbolSettingDialog* dialog;
	
	QCheckBox* oriented_to_north;
	ColorDropDown*  color_edit;
	QFontComboBox*  font_edit;
	QDoubleSpinBox* font_size_edit;
	QLineEdit*      letter_edit;
	QDoubleSpinBox* letter_size_edit;
	QCheckBox*      bold_check;
	QCheckBox*      italic_check;
	QCheckBox*      underline_check;
	QDoubleSpinBox* line_spacing_edit;
	QDoubleSpinBox* paragraph_spacing_edit;
	QDoubleSpinBox* character_spacing_edit;
	QCheckBox*      kerning_check;
	QLineEdit*      icon_text_edit;
	QCheckBox*      framing_check;
	QCheckBox*      ocad_compat_check;
	
	QWidget*        framing_widget;
	ColorDropDown*  framing_color_edit;
	QRadioButton*   framing_line_radio;
	QDoubleSpinBox* framing_line_half_width_edit;
	QRadioButton*   framing_shadow_radio;
	QDoubleSpinBox* framing_shadow_x_offset_edit;
	QDoubleSpinBox* framing_shadow_y_offset_edit;
	
	QWidget*        ocad_compat_widget;
	QCheckBox*      line_below_check;
	QDoubleSpinBox* line_below_width_edit;
	ColorDropDown*  line_below_color_edit;
	QDoubleSpinBox* line_below_distance_edit;
	QListWidget*    custom_tab_list;
	QPushButton*    custom_tab_add;
	QPushButton*    custom_tab_remove;
	
	bool react_to_changes;
};


}  // namespace OpenOrienteering

#endif
