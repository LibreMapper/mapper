/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Matthias Kühlewein (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "map_information_dialog.h"

#include <vector>

#include <Qt>
#include <QtGlobal>
#include <QAbstractButton>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFlags>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QIODevice>
#include <QLatin1String>
#include <QMessageBox>
#include <QPushButton>
#include <QSaveFile>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVarLengthArray>
#include <QVBoxLayout>

#include "core/map_information.h"
#include "gui/file_dialog.h"
#include "gui/main_window.h"


namespace LibreMapper {

namespace {

void setupTreeWidget(QTreeWidgetItem* root_item, const MapInformation& map_info)
{
	QVarLengthArray<QTreeWidgetItem*, 5> tree_item_hierarchy;
	tree_item_hierarchy.push_back(root_item);
	
	for (const auto &tree_item : map_info.treeItems())
	{
		auto const level = qMax(1, tree_item.level + 1);
		if (tree_item_hierarchy.size() > tree_item.level)
			tree_item_hierarchy.resize(level);
		
		auto* tree_widget_item = new QTreeWidgetItem(tree_item_hierarchy.back());
		tree_widget_item->setText(0, tree_item.label);
		tree_widget_item->setText(1, tree_item.value);
		tree_item_hierarchy.push_back(tree_widget_item);
	}
}

}  // namespace

MapInformationDialog::MapInformationDialog(MainWindow* parent, Map* map)
: QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
, main_window { parent }
, map_information { new MapInformation(map) }
{
	setWindowTitle(QCoreApplication::translate("LibreMapper::MapInformation", "Map information"));
	
	auto* save_button = new QPushButton(QIcon(QLatin1String(":/images/save.png")), QCoreApplication::translate("LibreMapper::MapInformation", "Save as..."));
	auto* close_button = new QPushButton(QGuiApplication::translate("QPlatformTheme", "Close"));
	close_button->setDefault(true);
	
	auto* buttons_layout = new QHBoxLayout();
	buttons_layout->addWidget(save_button);
	buttons_layout->addStretch(1);
	buttons_layout->addWidget(close_button);
	
	auto* map_info_tree = new QTreeWidget();
	map_info_tree->setColumnCount(2);
	map_info_tree->setHeaderHidden(true);
	
	setupTreeWidget(map_info_tree->invisibleRootItem(), *map_information);
	
	map_info_tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	
	auto* layout = new QVBoxLayout();
	layout->addWidget(map_info_tree);
	layout->addLayout(buttons_layout);
	setLayout(layout);
	resize(650, 600);
	
	connect(save_button, &QAbstractButton::clicked, this, &MapInformationDialog::save);
	connect(close_button, &QAbstractButton::clicked, this, &QDialog::accept);
}

MapInformationDialog::~MapInformationDialog() = default;


// slot
void MapInformationDialog::save()
{
	auto filepath = FileDialog::getSaveFileName(
	    this,
	    QCoreApplication::translate("LibreMapper::MainWindow", "Save file"),
	    QFileInfo(main_window->currentPath()).canonicalPath(),
	    QString::fromLatin1("%1 (*.txt)").arg(QCoreApplication::translate("LibreMapper::MapInformation", "Plain text")) );
	
	if (filepath.isEmpty())
		return;
	if (!filepath.endsWith(QLatin1String(".txt"), Qt::CaseInsensitive))
		filepath.append(QLatin1String(".txt"));
	
	QSaveFile file(filepath);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		file.write(map_information->makeTextReport().toUtf8());
		if (file.commit())
			return;
	}
	QMessageBox::warning(this, QCoreApplication::translate("LibreMapper::MapInformation", "Error"),
	                     QCoreApplication::translate("LibreMapper::MapEditorController", "Cannot save file\n%1:\n%2")
	                     .arg(filepath, file.errorString()) );
}


}  // namespace LibreMapper
