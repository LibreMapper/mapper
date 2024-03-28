/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_SYMBOL_WIDGET_H
#define OPENORIENTEERING_SYMBOL_WIDGET_H

#include <QObject>
#include <QScrollArea>

class QContextMenuEvent;
class QWidget;

namespace OpenOrienteering {

class Map;
class Symbol;
class SymbolRenderWidget;


/**
 * @brief Shows all symbols from a map in a scrollable widget.
 * 
 * SymbolWidget shows all symbols from a map's symbol set.
 * It lets the user select symbols and perform actions on symbols.
 * 
 * The implementation is based on SymbolRenderWidget and QScrollArea.
 * Normally it is used inside a dock widget.
 */
class SymbolWidget : public QScrollArea
{
Q_OBJECT
public:
	/**
	 * @brief Constructs a new SymbolWidget.
	 * @param map The map which provides the symbols. Must not be nullptr.
	 * @param mobile_mode If true, enables a special mode for mobile devices.
	 * @param parent The parent QWidget.
	 */
	SymbolWidget(Map* map, bool mobile_mode, QWidget* parent = nullptr);
	
	/**
	 * @brief Destroys the SymbolWidget.
	 */
	~SymbolWidget() override;
	
	/**
	 * @brief If exactly one symbol is selected, returns this symbol.
	 * 
	 * Otherwise returns nullptr.
	 */
	const Symbol* getSingleSelectedSymbol() const;
	
	/**
	 * @brief If exactly one symbol is selected, returns this symbol.
	 * 
	 * Otherwise returns nullptr.
	 */
	Symbol* getSingleSelectedSymbol();
	
	/**
	 * @brief Returns the number of selected symbols.
	 */
	int selectedSymbolsCount() const;
	
	/**
	 * @brief Checks if the symbol is selected.
	 */
	bool isSymbolSelected(const Symbol* symbol) const;
	
	/**
	 * @brief Selects the symbol exclusively, deselecting all other symbols.
	 */
	void selectSingleSymbol(const Symbol* symbol);
	
signals:
	/**
	 * @brief The collection of selected symbols changed.
	 */
	void selectedSymbolsChanged();
	
	/**
	 * @brief The user triggered "Switch symbol".
	 * @todo  Merge with/Reuse corresponding action in MapEditorController.
	 */
	void switchSymbolClicked();
	
	/**
	 * @brief The user triggered "Fill/Create border".
	 * @todo  Merge with/Reuse corresponding action in MapEditorController.
	 */
	void fillBorderClicked();
	
	/**
	 * @brief The user triggered selecting objects with the active symbol.
	 * @param select_exclusively If true, an existing selection is replaced,
	 *                           otherwise it is extend.
	 */
	void selectObjectsClicked(bool select_exclusively);
	
	/**
	 * @brief The user triggered deselecting objects with the active symbol.
	 */
	void deselectObjectsClicked();
	
protected:
	/**
	 * @brief Receives context menu events and opens the context menu.
	 */
	void contextMenuEvent(QContextMenuEvent* event) override;
	
private:
	SymbolRenderWidget* render_widget;
};


}  // namespace OpenOrienteering

#endif
