/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "item_delegates.h"

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QPainter>
#include <QSignalMapper>
#include <QSpinBox>
#include <QTextDocument>

#include "util/backports.h"  // IWYU pragma: keep
#include "gui/util_gui.h"


namespace OpenOrienteering {

// ### ColorItemDelegate ###

ColorItemDelegate::ColorItemDelegate(QObject* parent)
 : QStyledItemDelegate(parent)
{
	// Nothing.
}

void ColorItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (!option.state.testFlag(QStyle::State_Selected))
	{
		// Use the style options as is.
		QStyledItemDelegate::paint(painter, option, index);
	}
	else
	{
		// Use the model's background and/or foreground.
		QStyleOptionViewItem background_option(option);
		QPalette& palette(background_option.palette);
		
		QVariant background_var(index.data(Qt::BackgroundRole));
		if (!background_var.isNull())
		{
			palette.setColor(QPalette::Highlight, background_var.value<QColor>());
		}
		QVariant foreground_var(index.data(Qt::ForegroundRole));
		if (!foreground_var.isNull())
		{
			palette.setColor(QPalette::HighlightedText, foreground_var.value<QColor>());
		}
		QStyledItemDelegate::paint(painter, background_option, index);
		
		// Draw an extra frame.
		painter->save();
		painter->setPen(QPen(option.palette.color(QPalette::Highlight), 1));
		painter->drawRect(option.rect.adjusted(1, 1, -2, -2));
		painter->restore();
	}
}



//### SpinBoxDelegate ###

SpinBoxDelegate::SpinBoxDelegate(QObject* parent, int min, int max, const QString& unit, int step)
 : QItemDelegate(parent),
   min(min),
   max(max),
   step(step),
   unit(unit)
{
	// nothing
}

SpinBoxDelegate::SpinBoxDelegate(int min, int max, const QString& unit, int step)
 : QItemDelegate(),
   min(min),
   max(max),
   step(step),
   unit(unit)
{
	// nothing
}

QWidget* SpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);
	
	QSpinBox* spinbox = Util::SpinBox::create(min, max, unit, step);
	spinbox->setParent(parent);
	
	// Commit each change immediately when returning to event loop
	QSignalMapper* signal_mapper = new QSignalMapper(spinbox);
	signal_mapper->setMapping(spinbox, spinbox);
	connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), signal_mapper, QOverload<>::of(&QSignalMapper::map), Qt::QueuedConnection);
	connect(signal_mapper, QOverload<QWidget*>::of(&QSignalMapper::mapped), this, &QItemDelegate::commitData);
	
	return spinbox;
}

void SpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	int value = index.model()->data(index, Qt::UserRole).toInt();
	static_cast< QSpinBox* >(editor)->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QSpinBox* spinBox = static_cast< QSpinBox* >(editor);
	spinBox->interpretText();
	setModelData(model, index, spinBox->value());
}

void SpinBoxDelegate::setModelData(QAbstractItemModel* model, const QModelIndex& index, int value) const
{
	QMap< int, QVariant > data(model->itemData(index));
	data[Qt::UserRole] = value;
	data[Qt::DisplayRole] = unit.isEmpty() ?
	  QLocale().toString(value) : 
	  (QLocale().toString(value) + QLatin1Char(' ') + unit);
	model->setItemData(index, data);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(index);
	editor->setGeometry(option.rect);
}



//### PercentageDelegate ###

PercentageDelegate::PercentageDelegate(QObject* parent, int step)
 : QStyledItemDelegate(parent),
   step(step)
{
	unit = QLatin1Char(' ') + tr("%");
}

PercentageDelegate::PercentageDelegate(int step)
 : QStyledItemDelegate(),
   step(step)
{
	unit = QLatin1Char(' ') + tr("%");
}

QString PercentageDelegate::displayText(const QVariant& value, const QLocale& locale) const
{
	int int_value = qRound(value.toFloat() * 100);
	return locale.toString(int_value) + unit;
}

QWidget* PercentageDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);
	
	QSpinBox* spinbox = Util::SpinBox::create(0, 100, unit, step);
	spinbox->setParent(parent);
	
	// Commit each change immediately when returning to event loop
	connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, spinbox]() {
		emit const_cast<PercentageDelegate*>(this)->commitData(spinbox); 
	});
	
	return spinbox;
}

void PercentageDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	int value = qRound(index.model()->data(index, Qt::EditRole).toDouble() * 100);
	static_cast< QSpinBox* >(editor)->setValue(value);
}

void PercentageDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QSpinBox* spinBox = static_cast< QSpinBox* >(editor);
	spinBox->interpretText();
	model->setData(index, double(spinBox->value()) / 100.0, Qt::EditRole);
}

void PercentageDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(index);
	editor->setGeometry(option.rect);
}



// ### TextDocItemDelegate ###

TextDocItemDelegate::TextDocItemDelegate(QObject* parent, const Provider* provider)
: QStyledItemDelegate(parent)
, provider(provider)
{
	Q_ASSERT(provider);
}

TextDocItemDelegate::~TextDocItemDelegate()
{
	; // nothing
}

void TextDocItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QTextDocument* const text_doc = provider->textDoc(index);
	if (!text_doc)
	{
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}
	
	QStyleOptionViewItem options = option;
	initStyleOption(&options, index);
	
	QAbstractTextDocumentLayout::PaintContext context;
	if (options.state.testFlag(QStyle::State_Selected))
		context.palette.setColor(QPalette::Text, options.palette.color(QPalette::Active, QPalette::HighlightedText));
	else
		context.palette.setColor(QPalette::Text, options.palette.color(QPalette::Active, QPalette::Text));
	
	painter->save();
	
	const QStyle* const style = options.widget ? options.widget->style() : QApplication::style();
	style->drawControl(QStyle::CE_ItemViewItem, &options, painter);
	
	QRect rect = style->subElementRect(QStyle::SE_ItemViewItemText, &options);
	painter->translate(rect.topLeft());
	rect.moveTopLeft(QPoint());
	text_doc->documentLayout()->draw(painter, context);
	
	painter->restore();
}

QSize TextDocItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QTextDocument* const text_doc = provider->textDoc(index);
	if (!text_doc)
		return QStyledItemDelegate::sizeHint(option, index);
	
	return QSize(text_doc->idealWidth(), text_doc->size().height());
}


}  // namespace OpenOrienteering
