/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012, 2013, 2014, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "symbol_dropdown.h"

#include <Qt>
#include <QAbstractItemModel>
#include <QImage>
#include <QLatin1Char>
#include <QList>
#include <QMetaType>
#include <QModelIndex>
#include <QPixmap>
#include <QVariant>
#include <QWidget>

#include "core/map.h"
#include "core/symbols/combined_symbol.h"
#include "core/symbols/symbol.h"
#include "gui/util_gui.h"


// allow explicit use of Symbol pointers in QVariant
Q_DECLARE_METATYPE(LibreMapper::Symbol*)



namespace LibreMapper {

// ### SymbolDropDown ###


SymbolDropDown::SymbolDropDown(QWidget* parent)
 : QComboBox(parent)
{}

SymbolDropDown::SymbolDropDown(const Map* map, int filter, const Symbol* initial_symbol, const Symbol* excluded_symbol, QWidget* parent)
 : SymbolDropDown(parent)
{
	init(map, filter, initial_symbol, excluded_symbol);
}

SymbolDropDown::~SymbolDropDown() = default;


void SymbolDropDown::init(const Map* map, int filter, const Symbol* initial_symbol, const Symbol* excluded_symbol)
{
	addItem(tr("- none -"), QVariant::fromValue<const Symbol*>(nullptr));
	
	int size = map->getNumSymbols();
	for (int i = 0; i < size; ++i)
	{
		const auto* symbol = map->getSymbol(i);
		if (!(symbol->getType() & filter))
			continue;
		if (symbol == excluded_symbol)
			continue;
		if (symbol->getType() == Symbol::Combined)	// TODO: if point objects start to be able to contain objects of other ordinary symbols, add a check for these here, too, to prevent circular references
		{
			auto combined_symbol = static_cast<const CombinedSymbol*>(symbol);
			if (combined_symbol->containsSymbol(excluded_symbol))
				continue;
		}
		
		auto symbol_name = QString{symbol->getNumberAsString() + QLatin1Char(' ')
		                           + Util::plainText(map->translate(symbol->getName()))};
		addItem(QPixmap::fromImage(symbol->getIcon(map)), symbol_name, QVariant::fromValue<const Symbol*>(symbol));
	}
	setSymbol(initial_symbol);
}


const Symbol* SymbolDropDown::symbol() const
{
	auto data = itemData(currentIndex());
	if (data.canConvert<const Symbol*>())
		return data.value<const Symbol*>();
	else
		return nullptr;
}

void SymbolDropDown::setSymbol(const Symbol* symbol)
{
	setCurrentIndex(findData(QVariant::fromValue<const Symbol*>(symbol)));
}

void SymbolDropDown::addCustomItem(const QString& text, int id)
{
	insertItem(1 + num_custom_items, text, QVariant{id});
	++num_custom_items;
}

int SymbolDropDown::customID() const
{
	auto data = itemData(currentIndex());
	if (data.canConvert<int>())
		return data.value<int>();
	else
		return -1;
}

void SymbolDropDown::setCustomItem(int id)
{
	setCurrentIndex(findData(QVariant{id}));
}



// ### SymbolDropDownDelegate ###

SymbolDropDownDelegate::SymbolDropDownDelegate(int symbol_type_filter, QObject* parent)
: QItemDelegate{parent}
, symbol_type_filter{symbol_type_filter}
{
	// nothing else
}


SymbolDropDownDelegate::~SymbolDropDownDelegate() = default;



QWidget* SymbolDropDownDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const
{
	auto list = index.data(Qt::UserRole).toList();
	auto widget	= new SymbolDropDown(list.at(0).value<const Map*>(), symbol_type_filter,
	                                 list.at(1).value<const Symbol*>(), nullptr, parent);
	
	connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SymbolDropDownDelegate::emitCommitData);
	return widget;
}

void SymbolDropDownDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	auto widget = static_cast<SymbolDropDown*>(editor);
	auto symbol = index.data(Qt::UserRole).toList().at(1).value<const Symbol*>();
	widget->setSymbol(symbol);
}

void SymbolDropDownDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	auto widget = static_cast<SymbolDropDown*>(editor);
	auto symbol = widget->symbol();
	auto list = index.data(Qt::UserRole).toList();
	list[1] = QVariant::fromValue<const Symbol*>(symbol);
	model->setData(index, list, Qt::UserRole);
	
	if (symbol)
	{
		auto map = list.at(0).value<const Map*>();
		auto text = QString{symbol->getNumberAsString() + QLatin1Char(' ')
		                    + Util::plainText(map->translate(symbol->getName()))};
		model->setData(index, QVariant{text}, Qt::EditRole);
		model->setData(index, symbol->getIcon(list[0].value<const Map*>()), Qt::DecorationRole);
	}
	else
	{
		model->setData(index, tr("- None -"), Qt::EditRole);
		model->setData(index, {}, Qt::DecorationRole);
	}
}

void SymbolDropDownDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const
{
	editor->setGeometry(option.rect);
}

void SymbolDropDownDelegate::emitCommitData()
{
	emit commitData(qobject_cast<QWidget*>(sender()));
}


}  // namespace LibreMapper
