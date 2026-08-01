/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "template_table_model.h"

#include <memory>
#include <utility>

#include <Qt>
#include <QtGlobal>
#include <QBrush>
#include <QColor>
#include <QCoreApplication>
#include <QFlags>
#include <QIcon>
#include <QLatin1String>
#include <QModelIndex>
#include <QPalette>
#include <QMimeData>
#include <QUrl>
#include <QMessageBox>
#include <QList>
#include <QString>
#include <QtAssert>
#include <QtMinMax>
#include <QtPreprocessorSupport>

#include "core/map.h"
#include "core/map_view.h"
#include "templates/template.h"


namespace {

inline
Qt::CheckState toCheckState(const QVariant& value)
{
	return value.toBool() ? Qt::Checked : Qt::Unchecked;
}

QColor paletteColor(LibreMapper::MapView const& view, QPalette::ColorRole role) {
	auto const group = view.areAllTemplatesHidden() ? QPalette::Disabled : QPalette::Active;
	return QPalette().color(group, role);
}

/**
 * Provides unique combinations of column index and item data role.
 * 
 * This utility function allows more compact expressions (switch statements)
 * in the functions related item data.
 */
constexpr int combined(int column, Qt::ItemDataRole role)
{
	return int(role) * 256 + column + 256;
}

}


namespace LibreMapper {

TemplateTableModel::~TemplateTableModel() = default;

TemplateTableModel::TemplateTableModel(Map& map, MapView& view, QObject* parent)
: QAbstractTableModel(parent)
, map(map)
, view(view)
{
	connect(&map, &Map::firstFrontTemplateAboutToBeChanged, this, &TemplateTableModel::onFirstFrontTemplateAboutToBeChanged);
	connect(&map, &Map::firstFrontTemplateChanged, this, &TemplateTableModel::onFirstFrontTemplateChanged);
	connect(&map, &Map::templateAboutToBeAdded, this, &TemplateTableModel::onTemplateAboutToBeAdded);
	connect(&map, &Map::templateAdded, this, &TemplateTableModel::onTemplateAdded);
	connect(&map, &Map::templateChanged, this, &TemplateTableModel::onTemplateChanged);
	connect(&map, &Map::templateAboutToBeMoved, this, &TemplateTableModel::onTemplateAboutToBeMoved);
	connect(&map, &Map::templateMoved, this, &TemplateTableModel::onTemplateMoved);
	connect(&map, &Map::templateAboutToBeDeleted, this, &TemplateTableModel::onTemplateAboutToBeDeleted);
	connect(&map, &Map::templateDeleted, this, &TemplateTableModel::onTemplateDeleted);
	
	for (auto i = 0, last = map.getNumTemplates(); i < last; ++i)
	{
		auto* temp = map.getTemplate(i);
		connect(temp, &Template::templateStateChanged, this, &TemplateTableModel::onTemplateStateChanged);
	}
}


void TemplateTableModel::setCheckBoxDecorator(QVariant decorator)
{
	checkbox_decorator = decorator;
	emit dataChanged(this->index(0, visibilityColumn()), this->index(rowCount() - 1, visibilityColumn()));
}


void TemplateTableModel::setTouchMode(bool enabled)
{
	touch_mode = enabled;
	emit dataChanged(this->index(0, 0), this->index(rowCount() - 1, columnCount() - 1));
}


int TemplateTableModel::rowCount(const QModelIndex& parent) const
{
	return parent.isValid() ? 0 : map.getNumTemplates() + 1;
}

int TemplateTableModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 4;
}


int TemplateTableModel::posFromRow(int row) const
{
	Q_ASSERT(row >= 0 && row < rowCount());

	auto pos = map.getNumTemplates() - row;
	if (pos == map.getFirstFrontTemplate())
		pos = -1; // the map row
	else if (pos > map.getFirstFrontTemplate())
		--pos; // before the map row 
	
	return pos;
}

int TemplateTableModel::rowFromPos(int pos) const
{
	Q_ASSERT(pos >= 0);
	
	auto row = map.getNumTemplates() - pos;
	if (pos >= map.getFirstFrontTemplate())
		--row;
	
	return row;
}

int TemplateTableModel::insertionRowFromPos(int pos, bool in_background) const
{
	Q_ASSERT(pos >= 0);
	
	auto row = map.getNumTemplates() - pos;
	if (in_background)
		++row;
	
	return row;
}

int TemplateTableModel::mapRow(int first_front_template) const
{
	return map.getNumTemplates() - first_front_template;
}


QVariant TemplateTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case visibilityColumn():
			if (role == Qt::ToolTipRole)
				return QCoreApplication::translate("LibreMapper::TemplateListWidget", "Show");
			if (role == Qt::DecorationRole)
				return checkBoxDecorator();
			break;
		case opacityColumn():
			if (role == Qt::DisplayRole)
				return QCoreApplication::translate("LibreMapper::TemplateListWidget", "Opacity");
			break;
		case groupColumn():
			if (role == Qt::DisplayRole)
				return QCoreApplication::translate("LibreMapper::TemplateListWidget", "Group");
			break;
		case nameColumn():
			if (role == Qt::DisplayRole)
				return QCoreApplication::translate("LibreMapper::TemplateListWidget", "Filename");
			break;
		}
	}
	return {};
}

Qt::ItemFlags TemplateTableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid() || index.row() < 0 || index.row() > map.getNumTemplates() + 1)
		return Qt::ItemIsDropEnabled;

	auto pos = posFromRow(index.row());
	auto* temp = pos >= 0 ? map.getTemplate(pos) : nullptr;
	auto visibility = temp ? view.getTemplateVisibility(temp) : view.getMapVisibility();

	auto ret = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
	switch (index.column())
	{
	case visibilityColumn():
		ret |= Qt::ItemIsUserCheckable;
		break;
	case opacityColumn():
		ret |= ((visibility.visible && !touchMode()) ? Qt::ItemIsEditable : Qt::NoItemFlags);
		break;
	default:
		;
	}

	return ret;
}

QVariant TemplateTableModel::data(const QModelIndex &index, int role) const
{
	auto const pos = posFromRow(index.row());
	return pos < 0 ? mapData(index, role) : templateData(map.getTemplate(pos), index, role);
}

bool TemplateTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	auto const pos = posFromRow(index.row());
	return pos < 0 ? setMapData(index, value, role) : setTemplateData(map.getTemplate(pos), index, value, role);
}


bool TemplateTableModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
{
	Q_UNUSED(action);
	Q_UNUSED(row);
	Q_UNUSED(column);

	if (!data->hasUrls() || !data->hasFormat(QLatin1String("text/uri-list")) || parent.isValid())
		return false;

	for (auto const& url : data->urls())
	{
		if(!url.isLocalFile())
			return false;
	}

	return true;
}

bool TemplateTableModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	if (!canDropMimeData(data, action, row, column, parent))
		return false;

	if (action == Qt::IgnoreAction)
		return true;

	for (auto const& url : data->urls())
	{
		auto path = url.toLocalFile();

		QString error;
		auto new_template = Template::templateForPath(path, &map);
		if (!new_template)
		{
			error = tr("File format not recognized.");
		}
		else if (!new_template->setupAndLoad(nullptr, &view))
		{
			error = new_template->errorString();
			if (new_template->getTemplateState() == Template::Invalid && error.isEmpty())
				error = tr("Failed to load template. Does the file exist and is it valid?");
			new_template.reset();
		}

		if (!error.isEmpty())
		{
			auto const error_template = tr("Cannot open template\n%1:\n%2");
			QMessageBox::warning(nullptr, tr("Error"), error_template.arg(path, error));
		}

		if (new_template)
		{
			// row == -1 means that the file was dropped in the widget's free space
			// and row == 0 is the top of the list. Other values can be translated by posFromRow().
			auto pos = row > 0 ? posFromRow(row - 1) : (row == 0 ? map.getNumTemplates() : 0);
			map.addTemplate(pos, std::move(new_template));
		}
	}

	return true;
}


Qt::DropActions TemplateTableModel::supportedDropActions() const
{
	return Qt::CopyAction;
}


QVariant TemplateTableModel::mapData(const QModelIndex &index, int role) const
{
	if (role == Qt::BackgroundRole)
	{
#ifdef Q_OS_ANDROID
		auto background_color = paletteColor(view, QPalette::Background);
		auto r = (128 + 5 * background_color.red()) / 6;
		auto g = (128 + 5 * background_color.green()) / 6;
		auto b = (128 + 5 * background_color.blue()) / 6;
		return QBrush(QColor(r, g, b));
#else
		return QBrush(paletteColor(view, QPalette::AlternateBase));
#endif
	}
	
	auto visibility = view.getMapVisibility();
	if (role == Qt::ForegroundRole)
	{
		if (visibility.visible)
			return QBrush(paletteColor(view, QPalette::WindowText));
		return QBrush(QPalette().color(QPalette::Disabled, QPalette::WindowText));
	}
	
	switch (combined(index.column(), Qt::ItemDataRole(role)))
	{
	case combined(visibilityColumn(), Qt::CheckStateRole):
		return toCheckState(visibility.visible);
		
	case combined(opacityColumn(), Qt::DisplayRole):
	case combined(opacityColumn(), Qt::EditRole):
		return visibility.opacity;
		
	case combined(opacityColumn(), Qt::DecorationRole):
		if (visibility.visible)
			return QColor::fromCmykF(0, 0, 0, visibility.opacity);
		return QColor{Qt::transparent};
		
	case combined(visibilityColumn(), Qt::DisplayRole):
		if (!touchMode())
			break;
		Q_FALLTHROUGH();
	case combined(nameColumn(), Qt::DisplayRole):
		return QCoreApplication::translate("LibreMapper::TemplateListWidget", "- Map -");
	}
	return {};
}

QVariant TemplateTableModel::templateData(Template* temp, const QModelIndex &index, int role) const
{
	if (role == Qt::BackgroundRole)
	{
#ifdef Q_OS_ANDROID
		return QBrush(paletteColor(view, QPalette::Background));
#else
		return QBrush(paletteColor(view, QPalette::Base));
#endif
	}
	
	auto visibility = view.getTemplateVisibility(temp);
	if (role == Qt::ForegroundRole)
	{
		auto text_color = QColor::fromRgb(255, 51, 51);
		if (temp->getTemplateState() == Template::Invalid)
		{
			if (visibility.visible)
				text_color = text_color.darker();
		}
		else if (visibility.visible)
			text_color = paletteColor(view, QPalette::WindowText);
		else
			text_color = QPalette().color(QPalette::Disabled, QPalette::WindowText);
		return QBrush(text_color);
	}
	
	switch (combined(index.column(), Qt::ItemDataRole(role)))
	{
	case combined(visibilityColumn(), Qt::CheckStateRole):
		if (visibility.visible && temp->getTemplateState() != Template::Loaded)
			return Qt::PartiallyChecked;
		return toCheckState(visibility.visible);
		
	case combined(opacityColumn(), Qt::DisplayRole):
	case combined(opacityColumn(), Qt::EditRole):
		return visibility.opacity;
		
	case combined(opacityColumn(), Qt::DecorationRole):
		if (temp->getTemplateState() == Template::Invalid)
				return QIcon::fromTheme(QLatin1String("image-missing"), QIcon{QLatin1String(":/images/close.png")});
		if (visibility.visible)
			return QColor::fromCmykF(0, 0, 0, visibility.opacity);
		return QColor{Qt::transparent};
		
	case combined(visibilityColumn(), Qt::DisplayRole):
		if (!touchMode())
			break;
		Q_FALLTHROUGH();
	case combined(nameColumn(), Qt::DisplayRole):
		return temp->getTemplateFilename();
		
	case combined(visibilityColumn(), Qt::ToolTipRole):
		if (!touchMode())
			break;
		Q_FALLTHROUGH();
	case combined(nameColumn(), Qt::ToolTipRole):
		return temp->getTemplatePath();
	}
	
	if (role == Qt::UserRole)
	{
		
	}
	return {};
}


bool TemplateTableModel::setMapData(const QModelIndex& index, const QVariant& value, int role)
{
	switch (index.column())
	{
	case visibilityColumn():
		if (role == Qt::CheckStateRole)
		{
			auto vis = view.getMapVisibility();
			vis.visible = value.toInt() == Qt::Checked;
			view.setMapVisibility(vis);
			// Visibility impacts enabled state of the other fields in the row.
			emit dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1));
			return true;
		}
		break;
		
	case opacityColumn():
		if (role == Qt::EditRole)
		{
			auto vis = view.getMapVisibility();
			vis.opacity = qBound(0.0f, value.toFloat(), 1.0f);
			view.setMapVisibility(vis);
			emit dataChanged(index, index);
			return true;
		}
	}
	return false;
}

bool TemplateTableModel::setTemplateData(Template* temp, const QModelIndex& index, const QVariant& value, int role)
{
	switch (index.column())
	{
	case visibilityColumn():
		if (role == Qt::CheckStateRole)
		{
			auto vis = view.getTemplateVisibility(temp);
			vis.visible = value.toInt() == Qt::Checked;
			view.setTemplateVisibility(temp, vis);
			// Visibility impacts enabled state of the other fields in the row.
			emit dataChanged(this->index(index.row(), 0), this->index(index.row(), columnCount() - 1));
			return true;
		}
		break;
	case opacityColumn():
		if (role == Qt::EditRole)
		{
			auto vis = view.getTemplateVisibility(temp);
			vis.opacity = qBound(0.0f, value.toFloat(), 1.0f);
			view.setTemplateVisibility(temp, vis);
			emit dataChanged(index, index);
			return true;
		}
		break;
	}
	return false;
}


void TemplateTableModel::onFirstFrontTemplateAboutToBeChanged(int old_pos, int new_pos)
{
	// In this table, pos actually identifies the map row.
	auto old_row = mapRow(old_pos);
	auto new_row = mapRow(new_pos);
	if (new_row > old_row)
		++new_row;
	beginMoveRows({}, old_row, old_row, {}, new_row);
}

void TemplateTableModel::onFirstFrontTemplateChanged(int old_pos, int new_pos)
{
	// In this table, pos actually identifies the map row.
	auto old_row = mapRow(old_pos);
	auto new_row = mapRow(new_pos);
	if (new_row > old_row)
		++new_row;
	moveRows({}, old_row, 1, {}, new_row);
	endMoveRows();
}

void TemplateTableModel::onTemplateAboutToBeAdded(int pos, Template* /*temp*/, bool in_background)
{
	auto const row = insertionRowFromPos(pos, in_background);
	beginInsertRows({}, row, row);
}

void TemplateTableModel::onTemplateAdded(int /*pos*/, Template* temp)
{
	endInsertRows();
	connect(temp, &Template::templateStateChanged, this, &TemplateTableModel::onTemplateStateChanged);
}

void TemplateTableModel::onTemplateChanged(int pos, Template* temp)
{
	connect(temp, &Template::templateStateChanged, this, &TemplateTableModel::onTemplateStateChanged);
	auto const row = rowFromPos(pos);
	emit dataChanged(index(row, 0), index(row, 3));
}

void TemplateTableModel::onTemplateAboutToBeMoved(int old_pos, int new_pos)
{
	auto old_row = rowFromPos(old_pos);
	auto new_row = rowFromPos(new_pos);
	if (new_row > old_row)
		++new_row;
	beginMoveRows({}, old_row, old_row, {}, new_row);
}

void TemplateTableModel::onTemplateMoved()
{
	endMoveRows();
}

void TemplateTableModel::onTemplateAboutToBeDeleted(int pos, Template* temp)
{
	disconnect(temp);
	beginRemoveRows({}, rowFromPos(pos), rowFromPos(pos));
}

void TemplateTableModel::onTemplateDeleted()
{
	endRemoveRows();
}

void TemplateTableModel::onTemplateStateChanged()
{
	auto pos = this->map.findTemplateIndex(qobject_cast<Template*>(sender()));
	if (pos >= 0)
	{
		auto const row = rowFromPos(pos);
		emit dataChanged(index(row, 0), index(row, 3));
	}
}


}  // namespace LibreMapper
