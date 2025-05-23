/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2024 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "color_list_widget.h"

#include <Qt>
#include <QtGlobal>
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QColor>
#include <QDialog>
#include <QFlags>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLatin1Char>
#include <QLocale>
#include <QMargins>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QShowEvent>
#include <QString>
#include <QStringList>
#include <QStyle>
#include <QStyleOption>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVariant>

#include "core/map.h"
#include "core/map_color.h"
#include "core/symbols/symbol.h"
#include "gui/color_dialog.h"
#include "gui/main_window.h"
#include "gui/util_gui.h"
#include "gui/widgets/segmented_button_layout.h"
#include "util/item_delegates.h"

// IWYU pragma: no_forward_declare QTableWidgetItem


namespace LibreMapper {

namespace {

/// Local wrapper for Util::ToolButton::create() adding the What's This bit.
QToolButton* createToolButton(const QIcon& icon, const QString& text)
{
	return Util::ToolButton::create(icon, text, "color_dock_widget.html");
}

}  // anonymous namespace


ColorListWidget::ColorListWidget(Map* map, MainWindow* window, QWidget* parent)
: QWidget(parent)
, map(map)
, window(window)
{
	react_to_changes = true;
	
	// Color table
	color_table = new QTableWidget(map->getNumColorPrios(), 7);
	color_table->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::AnyKeyPressed);
	color_table->setSelectionMode(QAbstractItemView::SingleSelection);
	color_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	color_table->verticalHeader()->setVisible(false);
	color_table->setHorizontalHeaderLabels(QStringList() <<
	  QString{} << tr("Name") << tr("Spot color") << tr("CMYK") << tr("RGB") << tr("K.o.") << tr("Opacity") );
	color_table->setItemDelegateForColumn(0, new ColorItemDelegate(this));
	color_table->setItemDelegateForColumn(6, new PercentageDelegate(this));
	color_table->setColumnHidden(6, true);
	
	auto new_button_menu = new QMenu(this);
	(void) new_button_menu->addAction(tr("New"), this, SLOT(newColor()));
	duplicate_action = new_button_menu->addAction(tr("Duplicate"), this, SLOT(duplicateColor()));
	duplicate_action->setIcon(QIcon(QString::fromLatin1(":/images/tool-duplicate.png")));
	
	// Buttons
	auto new_button = createToolButton(QIcon(QString::fromLatin1(":/images/plus.png")), tr("New"));
	new_button->setPopupMode(QToolButton::DelayedPopup); // or MenuButtonPopup
	new_button->setMenu(new_button_menu);
	delete_button = createToolButton(QIcon(QString::fromLatin1(":/images/minus.png")), tr("Delete"));
	
	auto add_remove_layout = new SegmentedButtonLayout();
	add_remove_layout->addWidget(new_button);
	add_remove_layout->addWidget(delete_button);
	
	move_up_button = createToolButton(QIcon(QString::fromLatin1(":/images/arrow-up.png")), tr("Move Up"));
	move_up_button->setAutoRepeat(true);
	move_down_button = createToolButton(QIcon(QString::fromLatin1(":/images/arrow-down.png")), tr("Move Down"));
	move_down_button->setAutoRepeat(true);
	
	auto up_down_layout = new SegmentedButtonLayout();
	up_down_layout->addWidget(move_up_button);
	up_down_layout->addWidget(move_down_button);
	
	// TODO: In Mapper >= 0.6, switch to ColorWidget (or generic) translation context.
	edit_button = createToolButton(QIcon(QString::fromLatin1(":/images/settings.png")), QApplication::translate("LibreMapper::MapEditorController", "&Edit").remove(QLatin1Char('&')));
	edit_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	
	auto help_button = createToolButton(QIcon(QString::fromLatin1(":/images/help.png")), tr("Help"));
	help_button->setAutoRaise(true);
	
	// The buttons row layout
	auto buttons_group_layout = new QHBoxLayout();
	buttons_group_layout->addLayout(add_remove_layout);
	buttons_group_layout->addLayout(up_down_layout);
	buttons_group_layout->addWidget(edit_button);
	buttons_group_layout->addWidget(new QLabel(QString::fromLatin1("   ")), 1);
	buttons_group_layout->addWidget(help_button);
	
	// The layout of all components below the table
	auto bottom_layout = new QVBoxLayout();
	QStyleOption style_option(QStyleOption::Version, QStyleOption::SO_DockWidget);
	bottom_layout->setContentsMargins(
		style()->pixelMetric(QStyle::PM_LayoutLeftMargin, &style_option) / 2,
		0, // Covered by the main layout's spacing.
		style()->pixelMetric(QStyle::PM_LayoutRightMargin, &style_option) / 2,
		style()->pixelMetric(QStyle::PM_LayoutBottomMargin, &style_option) / 2
	);
	bottom_layout->addLayout(buttons_group_layout);
	bottom_layout->addWidget(new QLabel(tr("Double-click a color value to open a dialog.")));
	
	// The main layout
	auto layout = new QVBoxLayout();
	layout->setContentsMargins(QMargins());
	layout->addWidget(color_table, 1);
	layout->addLayout(bottom_layout);
	setLayout(layout);
	
	for (int i = 0; i < map->getNumColorPrios(); ++i)
		addRow(i);
	
	auto header_view = color_table->horizontalHeader();
	header_view->setSectionResizeMode(QHeaderView::Interactive);
	header_view->resizeSections(QHeaderView::ResizeToContents);
	header_view->setSectionResizeMode(0, QHeaderView::Fixed); // Color
	header_view->resizeSection(0, 32);
	header_view->setSectionResizeMode(5, QHeaderView::Fixed); // Knockout
	header_view->resizeSection(5, 32);
	header_view->setSectionsClickable(false);
	if (map->getNumColorPrios() == 0)
	{
		header_view->resizeSection(1, 160); // Name
		header_view->resizeSection(2, 128); // Spot colors
		header_view->resizeSection(3, 96);  // CMYK
		header_view->resizeSection(4, 64);  // RGB
	}
	
	currentCellChange(color_table->currentRow(), 0, 0, 0);	// enable / disable move color buttons
	
	// Connections
	connect(color_table, &QTableWidget::cellChanged, this, &ColorListWidget::cellChange);
	connect(color_table, &QTableWidget::currentCellChanged, this, &ColorListWidget::currentCellChange);
	connect(color_table, &QTableWidget::cellDoubleClicked, this, &ColorListWidget::editCurrentColor);
	
	connect(new_button, &QAbstractButton::clicked, this, &ColorListWidget::newColor);
	connect(delete_button, &QAbstractButton::clicked, this, &ColorListWidget::deleteColor);
	connect(move_up_button, &QAbstractButton::clicked, this, &ColorListWidget::moveColorUp);
	connect(move_down_button, &QAbstractButton::clicked, this, &ColorListWidget::moveColorDown);
	connect(edit_button, &QAbstractButton::clicked, this, &ColorListWidget::editCurrentColor);
	connect(help_button, &QAbstractButton::clicked, this, &ColorListWidget::showHelp);
	
	connect(map, &Map::colorAdded, this, &ColorListWidget::colorAdded);
	connect(map, &Map::colorChanged, this, &ColorListWidget::colorChanged);
	connect(map, &Map::colorDeleted, this, &ColorListWidget::colorDeleted);
}


ColorListWidget::~ColorListWidget() = default;



void ColorListWidget::showEvent(QShowEvent* event)
{
	if (!event->spontaneous())
	{
		// Update name, because translation may be changed with new symbol set 
		for (int i = 0, count = color_table->rowCount(); i < count; ++i)
		{
			auto color = map->getColorByPrio(i);
			auto item = color_table->item(i, 1);
			item->setText(map->translate(color->getName()));
		}
	}
}


void ColorListWidget::newColor()
{
	int row = color_table->currentRow();
	if (row < 0)
		row = color_table->rowCount();
	map->addColor(new MapColor(), row);
	
	map->updateAllObjects();
	
	editCurrentColor();
}

bool ColorListWidget::confirmColorDeletion(const MapColor* color_to_be_removed) const
{
	QString detailed_text;
	
	std::vector<const Symbol*> remaining_symbols;
	remaining_symbols.reserve(std::size_t(map->getNumSymbols()));
	{
		// First: direct usage in symbols
		QString direct_usage;
		map->applyOnAllSymbols(
			[&direct_usage, &remaining_symbols, color_to_be_removed](const Symbol* symbol) {
				if (symbol->getType() == Symbol::NoSymbol)
					return;
				if (symbol->containsColor(color_to_be_removed))
					direct_usage += symbol->getNumberAndPlainTextName() + QChar::LineFeed;
				else
					remaining_symbols.push_back(symbol);
			}
		);
		if (!direct_usage.isEmpty())
		{
			detailed_text += tr("This color is used by the following symbols:") + QChar::LineFeed
							 + direct_usage + QChar::LineFeed;
		}
	}
	
	if (color_to_be_removed->getSpotColorMethod() == MapColor::SpotColor)
	{
		// Second: usage as spot color
		QString spotcolor_usage;
		std::vector<const MapColor*> affected_colors;
		affected_colors.reserve(std::size_t(map->getNumColorPrios()));
		map->applyOnMatchingColors(
			[&spotcolor_usage, &affected_colors](const auto* color) {
				spotcolor_usage += color->getName() + QChar::LineFeed;
				affected_colors.push_back(color);
			},
			[&color_to_be_removed](const auto* color) {
				return color->hasSpotColorComponent(color_to_be_removed);
			}
		);
		if (!affected_colors.empty())
		{
			detailed_text += tr("This color is used as a spot color by the following map colors:") + QChar::LineFeed
							 + spotcolor_usage + QChar::LineFeed;
			
			// Third: Additional transitive usage via spot color composition
			QString transitive_usage;
			for (const auto* symbol : remaining_symbols)
			{
				for (const auto* color : affected_colors)
				{
					if (symbol->containsColor(color))
					{
						transitive_usage += symbol->getNumberAndPlainTextName() + QChar::LineFeed;
						break;
					}
				}
			}
			if (!transitive_usage.isEmpty())
			{
				detailed_text += tr("This spot color is used by the following symbols:") + QChar::LineFeed
								 + transitive_usage;
			}
		}
	}
		
	if (detailed_text.isEmpty())
		return true;
	
	QMessageBox msgBox;
	msgBox.setWindowTitle(tr("Confirmation"));
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(tr("Color \"%1\" is used by other elements. Removing the color will change the appearance of these elements. Do you really want to remove it?").arg(color_to_be_removed->getName()));
	msgBox.setDetailedText(detailed_text);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	return msgBox.exec() == QMessageBox::Yes;
}

void ColorListWidget::deleteColor()
{
	int row = color_table->currentRow();
	Q_ASSERT(row >= 0);
	if (row < 0) return; // In release mode
	
	// Show a warning if the color is used
	if (!confirmColorDeletion(map->getColorByPrio(row)))
		return;
	
	map->deleteColor(row);
	
	map->setColorsDirty();
	map->updateAllObjects();
}

void ColorListWidget::duplicateColor()
{
	int row = color_table->currentRow();
	Q_ASSERT(row >= 0);
	if (row < 0) return; // In release mode
	
	auto new_color = new MapColor(*map->getColorByPrio(row));
	//: Future replacement for COLOR_NAME + " (Duplicate)", for better localization.
	void(tr("%1 (duplicate)")); /// \todo Switch translation
	new_color->setName(map->translate(new_color->getName()) + tr(" (Duplicate)"));
	map->addColor(new_color, row);
	
	map->updateAllObjects();
	
	editCurrentColor();
}

void ColorListWidget::moveColorUp()
{
	int row = color_table->currentRow();
	Q_ASSERT(row >= 1);
	if (row < 1) return; // In release mode
	
	auto above_color = map->getMapColorByPrio(row - 1);
	auto cur_color = map->getMapColorByPrio(row);
	map->setColor(cur_color, row - 1);
	map->setColor(above_color, row);
	updateRow(row - 1);
	updateRow(row);
	
	color_table->setCurrentCell(row - 1, color_table->currentColumn());
	
	map->setColorsDirty();
	map->updateAllObjects();
}

void ColorListWidget::moveColorDown()
{
	int row = color_table->currentRow();
	Q_ASSERT(row < color_table->rowCount() - 1);
	if (row >= color_table->rowCount() - 1) return; // In release mode
	
	auto below_color = map->getMapColorByPrio(row + 1);
	auto cur_color = map->getMapColorByPrio(row);
	map->setColor(cur_color, row + 1);
	map->setColor(below_color, row);
	updateRow(row + 1);
	updateRow(row);
	
	color_table->setCurrentCell(row + 1, color_table->currentColumn());
	
	map->setColorsDirty();
	map->updateAllObjects();
}

// slot
void ColorListWidget::editCurrentColor()
{
	int row = color_table->currentRow();
	if (row >= 0)
	{
		auto color = map->getMapColorByPrio(row);
		ColorDialog dialog(*map, *color, this);
		dialog.setWindowModality(Qt::WindowModal);
		int result = dialog.exec();
		if (result == QDialog::Accepted)
		{
			*color = dialog.getColor();
			map->setColor(color, row); // trigger colorChanged signal
			map->setColorsDirty();
			map->updateAllObjects();
		}
	}
}

void ColorListWidget::showHelp() const
{
	Util::showHelp(window, "color_dock_widget.html");
}

void ColorListWidget::cellChange(int row, int column)
{
	if (!react_to_changes)
		return;
	
	react_to_changes = false;
	
	auto color = map->getMapColorByPrio(row);
	auto text = color_table->item(row, column)->text().trimmed();
	
	if (column == 1)
	{
		color->setName(text);
		react_to_changes = true;
	}
	else if (column == 6) // Opacity
	{
		auto opacity = color_table->item(row, column)->data(Qt::DisplayRole).toFloat();
		if (!qFuzzyCompare(1.0f+opacity, 1.0f+color->getOpacity()))
		{
			color->setOpacity(qBound(0.0f, opacity, 1.0f));
			updateRow(row);
		}
		react_to_changes = true;
	}
	else 
	{
		react_to_changes = true;
		return;
	}
	
	map->setColor(color, row); // trigger colorChanged signal
	map->setColorsDirty();
	map->updateAllObjects();
}

void ColorListWidget::currentCellChange(int current_row, int current_column, int previous_row, int previous_column)
{
	Q_UNUSED(current_column);
	Q_UNUSED(previous_row);
	Q_UNUSED(previous_column);
	if (!react_to_changes)
		return;
	
	bool valid_row = (current_row >= 0);
	delete_button->setEnabled(valid_row);
	duplicate_action->setEnabled(valid_row);
	move_up_button->setEnabled(valid_row && current_row >= 1);
	move_down_button->setEnabled(valid_row && current_row < color_table->rowCount() - 1);
	edit_button->setEnabled(valid_row);
}

void ColorListWidget::colorAdded(int index, const MapColor* color)
{
	Q_UNUSED(color);
	color_table->insertRow(index);
	addRow(index);
	if (index < color_table->rowCount() - 1)
	{
		updateRow(index + 1);
	}
	color_table->setCurrentCell(index, 1);
}

void ColorListWidget::colorChanged(int index, const MapColor* color)
{
	Q_UNUSED(color);
	updateRow(index);
}

void ColorListWidget::colorDeleted(int index, const MapColor* color)
{
	Q_UNUSED(color);
	color_table->removeRow(index);
	currentCellChange(color_table->currentRow(), color_table->currentColumn(), -1, -1);
}

void ColorListWidget::addRow(int row)
{
	react_to_changes = false;
	
	QTableWidgetItem* item;
	for (int col = 0; col < color_table->columnCount(); ++col)
	{
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		// TODO: replace "define" with "edit"
		item->setToolTip(tr("Double click to define the color"));
		color_table->setItem(row, col, item);
	}
	
#if 0 // Interferes with translation
	// Name
	item = color_table->item(row, 1);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	item->setToolTip(tr("Click to select the name and click again to edit."));
#endif
	
	// Opacity
	item = color_table->item(row, 6);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	
	react_to_changes = true;
	
	updateRow(row);
}

void ColorListWidget::updateRow(int row)
{
	react_to_changes = false;
	
	const auto* color = map->getColorByPrio(row);
	auto color_with_opacity = colorWithOpacity(*color);
	
	// Color preview
	auto item = color_table->item(row, 0);
	item->setBackground(color_with_opacity);
	
	// Name
	item = color_table->item(row, 1);
	item->setText(map->translate(color->getName()));
	
	// Spot color
	item = color_table->item(row, 2);
	item->setText(color->getSpotColorName());
	switch (color->getSpotColorMethod())
	{
		case MapColor::SpotColor:
			if (color->getScreenFrequency() > 0)
			{
				QLocale locale;
				item->setText(tr("%1 (%2°, %3 lpi)")
				              .arg(color->getSpotColorName(),
				                   locale.toString(color->getScreenAngle(), 'f', 1),
				                   locale.toString(color->getScreenFrequency(), 'f', 1)));
			}
			item->setData(Qt::DecorationRole, color_with_opacity);
			break;
		default:
			item->setData(Qt::DecorationRole, QColor(Qt::transparent));
	}
	
	// CMYK
	item = color_table->item(row, 3);
	item->setToolTip(tr("Double click to define the color"));
	const MapColorCmyk& cmyk = color->getCmyk();
	QLocale l;
	item->setText(QString::fromLatin1("%1/%2/%3/%4").arg(
	                l.toString(100*cmyk.c, 'g', 3), l.toString(100*cmyk.m, 'g', 3),
	                l.toString(100*cmyk.y, 'g', 3), l.toString(100*cmyk.k, 'g', 3)));
	switch (color->getCmykColorMethod())
	{
		case MapColor::SpotColor:
		case MapColor::RgbColor:
			item->setForeground(palette().color(QPalette::Disabled, QPalette::Text));
			item->setData(Qt::DecorationRole, QColor(Qt::transparent));
			break;
		default:
			item->setForeground(palette().color(QPalette::Active, QPalette::Text));
			item->setData(Qt::DecorationRole, colorWithOpacity(color->getCmyk(), color->getOpacity()));
	}
	
	// RGB
	item = color_table->item(row, 4);
	item->setText(QColor(color->getRgb()).name());
	item->setToolTip(item->text());
	switch (color->getRgbColorMethod())
	{
		case MapColor::SpotColor:
		case MapColor::CmykColor:
			item->setForeground(palette().color(QPalette::Disabled, QPalette::Text));
			item->setData(Qt::DecorationRole, QColor(Qt::transparent));
			break;
		default:
			item->setForeground(palette().color(QPalette::Active, QPalette::Text));
			item->setData(Qt::DecorationRole, colorWithOpacity(color->getRgb(), color->getOpacity()));
	}
	
	// Knockout
	item = color_table->item(row, 5);
	item->setCheckState(color->getKnockout() ? Qt::Checked : Qt::Unchecked);
	item->setForeground(palette().color(QPalette::Disabled, QPalette::Text));
	
	// Opacity
	item = color_table->item(row, 6);
	item->setData(Qt::DisplayRole, color->getOpacity());
	
	react_to_changes = true;
}


}  // namespace LibreMapper
