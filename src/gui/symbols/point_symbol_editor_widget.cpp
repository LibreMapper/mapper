/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2019, 2024, 2025 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "point_symbol_editor_widget.h"

#include <algorithm>
#include <limits>
// IWYU pragma: no_include <memory>

#include <Qt>
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QCursor>
#include <QDoubleSpinBox>
#include <QFlags>
#include <QFontMetrics>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLocale>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <QPointF>
#include <QPushButton>
#include <QRectF>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVariant>

#include "core/map.h"
#include "core/objects/object.h"
#include "core/symbols/area_symbol.h"
#include "core/symbols/line_symbol.h"
#include "core/symbols/point_symbol.h"
#include "core/symbols/symbol.h"
#include "gui/modifier_key.h"
#include "gui/map/map_editor.h"
#include "gui/map/map_widget.h"
#include "gui/util_gui.h"
#include "gui/widgets/color_dropdown.h"
#include "util/scoped_signals_blocker.h"


namespace LibreMapper {

PointSymbolEditorWidget::PointSymbolEditorWidget(MapEditorController* controller, PointSymbol* symbol, qreal offset_y, bool permanent_preview, QWidget* parent)
: QWidget(parent)
, symbol(symbol)
, object_origin_coord(0, offset_y)
, offset_y(offset_y)
, controller(controller)
, permanent_preview(permanent_preview)
{
	map = controller->getMap();
	
	if (permanent_preview)
	{
		midpoint_object = new PointObject(symbol);
		midpoint_object->setPosition(object_origin_coord);
		map->addObject(midpoint_object);
	}
	
	oriented_to_north = new QCheckBox(tr("Always oriented to north (not rotatable)"));
	oriented_to_north->setChecked(!symbol->isRotatable());
	
	auto* elements_label = Util::Headline::create(tr("Elements"));
	element_list = new QListWidget();
	initElementList();
	
	delete_element_button = new QPushButton(QIcon(QString::fromLatin1(":/images/minus.png")), QString{});
	auto* add_element_button = new QToolButton();
	add_element_button->setIcon(QIcon(QString::fromLatin1(":/images/plus.png")));
	add_element_button->setToolButtonStyle(Qt::ToolButtonIconOnly);
	add_element_button->setPopupMode(QToolButton::InstantPopup);
	add_element_button->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
	add_element_button->setMinimumSize(delete_element_button->sizeHint());
	auto* add_element_button_menu = new QMenu(add_element_button);
	add_element_button_menu->addAction(tr("Point"), this, SLOT(addPointClicked()));
	add_element_button_menu->addAction(tr("Line"), this, SLOT(addLineClicked()));
	add_element_button_menu->addAction(tr("Area"), this, SLOT(addAreaClicked()));
	add_element_button->setMenu(add_element_button_menu);
	
	center_all_elements_button = new QPushButton(tr("Center all elements"));
	
	auto* current_element_label = Util::Headline::create(tr("Current element"));
	
	element_properties_widget = new QStackedWidget();
	
	// Point (circle)
	point_properties = new QWidget();
	auto* point_inner_radius_label = new QLabel(tr("Diameter <b>a</b>:"));
	point_inner_radius_edit = Util::SpinBox::create(2, 0.0, 99999.9, tr("mm"));
	
	auto* point_inner_color_label = new QLabel(tr("Inner color:"));
	point_inner_color_edit = new ColorDropDown(map);
	
	auto* point_outer_width_label = new QLabel(tr("Outer width <b>b</b>:"));
	point_outer_width_edit = Util::SpinBox::create(2, 0.0, 99999.9, tr("mm"));
	
	auto* point_outer_color_label = new QLabel(tr("Outer color:"));
	point_outer_color_edit = new ColorDropDown(map);
	
	auto* explanation_label = new QLabel();
	explanation_label->setPixmap(QPixmap(QString::fromLatin1(":/images/symbol_point_explanation.png")));
	
	auto* point_layout = new QGridLayout();
	point_layout->setContentsMargins(0, 0, 0, 0);
	point_layout->addWidget(point_inner_radius_label, 0, 0);
	point_layout->addWidget(point_inner_radius_edit, 0, 1);
	point_layout->addWidget(point_inner_color_label, 1, 0);
	point_layout->addWidget(point_inner_color_edit, 1, 1);
	point_layout->addWidget(new QWidget(), 2, 0, 1, -1);
	point_layout->addWidget(point_outer_width_label, 3, 0);
	point_layout->addWidget(point_outer_width_edit, 3, 1);
	point_layout->addWidget(point_outer_color_label, 4, 0);
	point_layout->addWidget(point_outer_color_edit, 4, 1);
	point_layout->addWidget(explanation_label, 0, 2, 5, 1);
	point_layout->setRowStretch(6, 1);
	point_layout->setColumnStretch(1,1);
	point_properties->setLayout(point_layout);
	element_properties_widget->addWidget(point_properties);
	
	// Line
	line_properties = new QWidget();
	auto* line_width_label = new QLabel(tr("Line width:"));
	line_width_edit = Util::SpinBox::create(3, 0.0, 99999.9, tr("mm"));
	
	auto* line_color_label = new QLabel(tr("Line color:"));
	line_color_edit = new ColorDropDown(map);
	
	auto* line_cap_label = new QLabel(tr("Line cap:"));
	line_cap_edit = new QComboBox();
	line_cap_edit->addItem(tr("flat"), QVariant(LineSymbol::FlatCap));
	line_cap_edit->addItem(tr("round"), QVariant(LineSymbol::RoundCap));
	line_cap_edit->addItem(tr("square"), QVariant(LineSymbol::SquareCap));
	//line_cap_edit->addItem(tr("pointed"), QVariant(LineSymbol::PointedCap));	// this would require another input field for the cap length
	
	auto* line_join_label = new QLabel(tr("Line join:"));
	line_join_edit = new QComboBox();
	line_join_edit->addItem(tr("miter"), QVariant(LineSymbol::MiterJoin));
	line_join_edit->addItem(tr("round"), QVariant(LineSymbol::RoundJoin));
	line_join_edit->addItem(tr("bevel"), QVariant(LineSymbol::BevelJoin));
	
	line_closed_check = new QCheckBox(tr("Line closed"));
	
	auto* line_layout = new QGridLayout();
	line_layout->setContentsMargins(0, 0, 0, 0);
	line_layout->addWidget(line_width_label, 0, 0);
	line_layout->addWidget(line_width_edit, 0, 1);
	line_layout->addWidget(line_color_label, 1, 0);
	line_layout->addWidget(line_color_edit, 1, 1);
	line_layout->addWidget(line_cap_label, 2, 0);
	line_layout->addWidget(line_cap_edit, 2, 1);
	line_layout->addWidget(line_join_label, 3, 0);
	line_layout->addWidget(line_join_edit, 3, 1);
	line_layout->addWidget(line_closed_check, 4, 0, 1, 2);
	line_layout->setRowStretch(5, 1);
	line_layout->setColumnStretch(1,1);
	line_properties->setLayout(line_layout);
	element_properties_widget->addWidget(line_properties);
	
	// Area
	area_properties = new QWidget();
	auto* area_color_label = new QLabel(tr("Area color:"));
	area_color_edit = new ColorDropDown(map);
	
	auto* area_layout = new QGridLayout();
	area_layout->setContentsMargins(0, 0, 0, 0);
	area_layout->addWidget(area_color_label, 0, 0);
	area_layout->addWidget(area_color_edit, 0, 1);
	area_layout->setRowStretch(1, 1);
	area_layout->setColumnStretch(1,1);
	area_properties->setLayout(area_layout);
	element_properties_widget->addWidget(area_properties);
	
	// Coordinates
	coords_label = new QLabel(tr("Coordinates:"));
	
	coords_table = new QTableWidget(0, 3);
	coords_table->setEditTriggers(QAbstractItemView::AllEditTriggers);
	coords_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	coords_table->setHorizontalHeaderLabels(QStringList() << tr("X") << tr("Y") << tr("Curve start"));
	coords_table->verticalHeader()->setVisible(false);
	
	auto* header_view = coords_table->horizontalHeader();
	header_view->setSectionResizeMode(0, QHeaderView::Interactive);
	header_view->setSectionResizeMode(1, QHeaderView::Interactive);
	header_view->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	header_view->setSectionsClickable(false);
	
	add_coord_button = new QPushButton(QIcon(QString::fromLatin1(":/images/plus.png")), QString{});
	delete_coord_button = new QPushButton(QIcon(QString::fromLatin1(":/images/minus.png")), QString{});
	center_coords_button = new QPushButton(tr("Center by coordinate average"));
	
	// Layout
	auto* left_layout = new QVBoxLayout();
	
	left_layout->addWidget(elements_label);
	left_layout->addWidget(element_list, 1);
	auto* element_buttons_layout = new QGridLayout();
	element_buttons_layout->setColumnStretch(0, 1);
	element_buttons_layout->addWidget(add_element_button, 1, 2);
	element_buttons_layout->addWidget(delete_element_button, 1, 3);
	element_buttons_layout->addWidget(center_all_elements_button, 2, 1, 1, 4);
	element_buttons_layout->setColumnStretch(5, 1);
	left_layout->addLayout(element_buttons_layout);
	
	auto* right_layout = new QVBoxLayout();
	right_layout->setContentsMargins({});
	
	right_layout->addWidget(current_element_label);
	right_layout->addWidget(element_properties_widget);
	right_layout->addSpacing(16);
	
	right_layout->addWidget(coords_label);
	right_layout->addWidget(coords_table);
	auto* coords_buttons_layout = new QHBoxLayout();
	coords_buttons_layout->addWidget(add_coord_button);
	coords_buttons_layout->addWidget(delete_coord_button);
	coords_buttons_layout->addStretch(1);
	coords_buttons_layout->addWidget(center_coords_button);
	right_layout->addLayout(coords_buttons_layout);
	
	auto* columns_layout = new QHBoxLayout;
	columns_layout->addLayout(left_layout);
	columns_layout->addSpacing(16);
	columns_layout->addLayout(right_layout);
	
	auto* layout = new QVBoxLayout();
	layout->addWidget(oriented_to_north);
	layout->addSpacerItem(Util::SpacerItem::create(this));
	layout->addLayout(columns_layout);
	setLayout(layout);
	
	// Connections
	connect(oriented_to_north, &QCheckBox::clicked, this, &PointSymbolEditorWidget::orientedToNorthClicked);
	
	connect(element_list, &QListWidget::currentRowChanged, this, &PointSymbolEditorWidget::changeElement);
	connect(delete_element_button, &QPushButton::clicked, this, &PointSymbolEditorWidget::deleteCurrentElement);
	connect(center_all_elements_button, &QPushButton::clicked, this, &PointSymbolEditorWidget::centerAllElements);
	
	connect(point_inner_radius_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PointSymbolEditorWidget::pointInnerRadiusChanged);
	connect(point_inner_color_edit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PointSymbolEditorWidget::pointInnerColorChanged);
	connect(point_outer_width_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PointSymbolEditorWidget::pointOuterWidthChanged);
	connect(point_outer_color_edit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PointSymbolEditorWidget::pointOuterColorChanged);
	
	connect(line_width_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PointSymbolEditorWidget::lineWidthChanged);
	connect(line_color_edit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PointSymbolEditorWidget::lineColorChanged);
	connect(line_cap_edit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PointSymbolEditorWidget::lineCapChanged);
	connect(line_join_edit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PointSymbolEditorWidget::lineJoinChanged);
	connect(line_closed_check, &QCheckBox::clicked, this, &PointSymbolEditorWidget::lineClosedClicked);
	
	connect(area_color_edit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PointSymbolEditorWidget::areaColorChanged);
	
	connect(coords_table, &QTableWidget::currentCellChanged, this, &PointSymbolEditorWidget::updateDeleteCoordButton);
	connect(coords_table, &QTableWidget::cellChanged, this, &PointSymbolEditorWidget::coordinateChanged);
	connect(add_coord_button, &QPushButton::clicked, this, &PointSymbolEditorWidget::addCoordClicked);
	connect(delete_coord_button, &QPushButton::clicked, this, &PointSymbolEditorWidget::deleteCoordClicked);
	connect(center_coords_button, &QPushButton::clicked, this, &PointSymbolEditorWidget::centerCoordsClicked);
}

PointSymbolEditorWidget::~PointSymbolEditorWidget()
{
	if (isVisible())
		setEditorActive(false);
	if (permanent_preview)
		map->deleteObject(midpoint_object);
}

void PointSymbolEditorWidget::setEditorActive(bool active)
{
	if (active)
	{
		if (!permanent_preview && !midpoint_object)
		{
			midpoint_object = new PointObject(symbol);
			midpoint_object->setPosition(object_origin_coord);
			map->addObject(midpoint_object);
		}
		map->updateAllObjectsWithSymbol(symbol);
		controller->setTool(new PointSymbolEditorTool(controller, this));
		activity = new PointSymbolEditorActivity(map, this);
		controller->setEditorActivity(activity);
		changeElement(element_list->currentRow());
	}
	else
	{
		controller->setTool(nullptr);
		controller->setEditorActivity(nullptr);
		if (!permanent_preview && midpoint_object)
		{
			map->deleteObject(midpoint_object);
			midpoint_object = nullptr;
		}
	}
}

void PointSymbolEditorWidget::setVisible(bool visible)
{
	setEditorActive(visible);
	QWidget::setVisible(visible);
}

bool PointSymbolEditorWidget::changeCurrentCoordinate(const MapCoordF& new_coord)
{
	auto* object = getCurrentElementObject();
	if (object == midpoint_object)
		return false;
	
	if (object->getType() == Object::Point)
	{
		auto* point = static_cast<PointObject*>(object);
		auto coord = point->getCoordF();
		coord.setX(new_coord.x());
		coord.setY(new_coord.y() - offset_y);
		point->setPosition(coord);
	}
	else
	{
		auto table_row = coords_table->currentRow();
		if (table_row < 0)
			return false;
		
		auto* path = object->asPath();
		
		auto coord_index = MapCoordVector::size_type(table_row);
		Q_ASSERT(coord_index < path->getCoordinateCount());
		
		auto coord = path->getCoordinate(coord_index);
		coord.setX(new_coord.x());
		coord.setY(new_coord.y() - offset_y);
		path->setCoordinate(coord_index, coord);
	}
	
	updateCoordsTable();
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
	return true;
}

bool PointSymbolEditorWidget::addCoordinate(const MapCoordF& new_coord)
{
	auto* object = getCurrentElementObject();
	if (object == midpoint_object)
		return false;
	
	if (object->getType() == Object::Point)
		return changeCurrentCoordinate(new_coord);
	Q_ASSERT(object->getType() == Object::Path);
	auto* path = static_cast<PathObject*>(object);
	
	auto table_row = coords_table->currentRow();
	if (table_row < 0)
		table_row = coords_table->rowCount();
	else
		++table_row;
	
	auto coord_index = MapCoordVector::size_type(table_row);
	path->addCoordinate(coord_index, { new_coord.x(), new_coord.y() - offset_y });
	
	if (path->getCoordinateCount() == 1)
	{
		if (object->getSymbol()->getType() == Symbol::Area)
			path->parts().front().setClosed(true, false);
	}
	
	updateCoordsTable();
	coords_table->setCurrentItem(coords_table->item(table_row, (coords_table->currentColumn() < 0) ? 0 : coords_table->currentColumn()));
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
	return true;
}

void PointSymbolEditorWidget::initElementList()
{
	element_list->clear();
	element_list->addItem(tr("[Midpoint]"));	// NOTE: Is that item needed?
	for (int i = 0; i < symbol->getNumElements(); ++i)
	{
		auto* element_symbol = symbol->getElementSymbol(i);
		element_list->addItem(getLabelForSymbol(element_symbol));
	}
	element_list->setCurrentRow(0);
}

void PointSymbolEditorWidget::orientedToNorthClicked(bool checked)
{
	symbol->setRotatable(!checked);
	emit symbolEdited();
}

void PointSymbolEditorWidget::changeElement(int row)
{
	delete_element_button->setEnabled(row > 0);	// must not remove first row
	center_all_elements_button->setEnabled(symbol->getNumElements() > 0);
	
	if (row >= 0)
	{
		auto* element_symbol = getCurrentElementSymbol();
		auto* object = getCurrentElementObject();
		
		if (object->getType() == Object::Path)
		{
			auto* path = static_cast<PathObject*>(object);
			if (element_symbol->getType() == Symbol::Line)
			{
				{
					const ScopedMultiSignalsBlocker block(
					  line_width_edit,
					  line_color_edit,
					  line_cap_edit,
					  line_join_edit,
					  line_closed_check
					);
					
					auto* line = static_cast<LineSymbol*>(element_symbol);
					line_width_edit->setValue(0.001 * line->getLineWidth());
					line_color_edit->setColor(line->getColor());
					line_cap_edit->setCurrentIndex(line_cap_edit->findData(QVariant(line->getCapStyle())));
					line_join_edit->setCurrentIndex(line_join_edit->findData(QVariant(line->getJoinStyle())));
					
					const auto& parts = path->parts();
					line_closed_check->setChecked(!parts.empty() && parts.front().isClosed());
					line_closed_check->setEnabled(!parts.empty());
				}
				
				element_properties_widget->setCurrentWidget(line_properties);
			}
			else if (element_symbol->getType() == Symbol::Area)
			{
				{
					const QSignalBlocker blocker(area_color_edit);
					
					auto* area = static_cast<AreaSymbol*>(element_symbol);
					area_color_edit->setColor(area->getColor());
				}
				
				element_properties_widget->setCurrentWidget(area_properties);
			}
			
			coords_label->setEnabled(true);
			coords_table->setEnabled(true);
			coords_table->setColumnHidden(2, false);
			add_coord_button->setEnabled(true);
			center_coords_button->setEnabled(path->getCoordinateCount() > 0);
		}
		else
		{
			{
				const ScopedMultiSignalsBlocker block(
				  point_inner_radius_edit,
				  point_inner_color_edit,
				  point_outer_width_edit,
				  point_outer_color_edit
				);
				
				auto* point = static_cast<PointSymbol*>(element_symbol);
				point_inner_radius_edit->setValue(2 * 0.001 * point->getInnerRadius());
				point_inner_color_edit->setColor(point->getInnerColor());
				point_outer_width_edit->setValue(0.001 * point->getOuterWidth());
				point_outer_color_edit->setColor(point->getOuterColor());
			}
			
			element_properties_widget->setCurrentWidget(point_properties);
			
			coords_table->setColumnHidden(2, true);
			coords_label->setEnabled(row > 0);
			coords_table->setEnabled(row > 0);
			add_coord_button->setEnabled(false);
			center_coords_button->setEnabled(row > 0);
		}
		
		coords_table->clearContents();
	}
	
	if (row > 0)
		updateCoordsTable();
	else
		coords_table->setRowCount(0);
	
	delete_coord_button->setEnabled(false);
}

void PointSymbolEditorWidget::addPointClicked()
{
	auto* new_point = new PointSymbol();
	auto* new_object = new PointObject(new_point);
	
	insertElement(new_object, new_point);
}

void PointSymbolEditorWidget::addLineClicked()
{
	auto* new_line = new LineSymbol();
	auto* new_object = new PathObject(new_line);
	
	insertElement(new_object, new_line);
}

void PointSymbolEditorWidget::addAreaClicked()
{
	auto* new_area = new AreaSymbol();
	auto* new_object = new PathObject(new_area);
	
	insertElement(new_object, new_area);
}

void PointSymbolEditorWidget::deleteCurrentElement()
{
	int row = element_list->currentRow();
	Q_ASSERT(row > 0);
	delete element_list->item(row);
	symbol->deleteElement(row - 1);
	center_all_elements_button->setEnabled(symbol->getNumElements() > 0);
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::centerAllElements()
{
	bool has_coordinates = false;
	auto min_x = std::numeric_limits<qint32>::max();
	auto max_x = std::numeric_limits<qint32>::min();
	auto min_y = std::numeric_limits<qint32>::max();
	auto max_y = std::numeric_limits<qint32>::min();
	
	for (int i = 0; i < symbol->getNumElements(); ++i)
	{
		auto* object = symbol->getElementObject(i);
		for (const auto& coord : object->getRawCoordinateVector())
		{
			min_x = std::min(min_x, coord.nativeX());
			min_y = std::min(min_y, coord.nativeY());
			max_x = std::max(max_x, coord.nativeX());
			max_y = std::max(max_y, coord.nativeY());
			has_coordinates = true;
		}
	}
	
	if (has_coordinates)
	{
		auto center_x = (min_x + max_x) / 2;
		auto center_y = (min_y + max_y) / 2;
		
		for (int i = 0; i < symbol->getNumElements(); ++i)
		{
			auto* object = symbol->getElementObject(i);
			object->move(-center_x, -center_y);
			object->update();
		}
	}
	
	if (element_list->currentRow() > 0)
		updateCoordsTable();
	emit symbolEdited();
}

void PointSymbolEditorWidget::pointInnerRadiusChanged(double value)
{
	auto* symbol = static_cast<PointSymbol*>(getCurrentElementSymbol());
	symbol->inner_radius = qRound(1000 * 0.5 * value);
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::pointInnerColorChanged()
{
	auto* symbol = static_cast<PointSymbol*>(getCurrentElementSymbol());
	symbol->inner_color = point_inner_color_edit->color();
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::pointOuterWidthChanged(double value)
{
	auto* symbol = static_cast<PointSymbol*>(getCurrentElementSymbol());
	symbol->outer_width = qRound(1000 * value);
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::pointOuterColorChanged()
{
	auto* symbol = static_cast<PointSymbol*>(getCurrentElementSymbol());
	symbol->outer_color = point_outer_color_edit->color();
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::lineWidthChanged(double value)
{
	auto* symbol = static_cast<LineSymbol*>(getCurrentElementSymbol());
	symbol->line_width = qRound(1000 * value);
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::lineColorChanged()
{
	auto* symbol = static_cast<LineSymbol*>(getCurrentElementSymbol());
	symbol->color = line_color_edit->color();
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::lineCapChanged(int index)
{
	auto* symbol = static_cast<LineSymbol*>(getCurrentElementSymbol());
	symbol->cap_style = static_cast<LineSymbol::CapStyle>(line_cap_edit->itemData(index).toInt());
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::lineJoinChanged(int index)
{
	auto* symbol = static_cast<LineSymbol*>(getCurrentElementSymbol());
	symbol->join_style = static_cast<LineSymbol::JoinStyle>(line_join_edit->itemData(index).toInt());
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::lineClosedClicked(bool checked)
{
	auto* object = getCurrentElementObject();
	Q_ASSERT(object->getType() == Object::Path);
	auto* path = static_cast<PathObject*>(object);
	
	if (!checked && path->getCoordinateCount() >= 4 && path->getCoordinate(path->getCoordinateCount() - 4).isCurveStart())
		path->getCoordinateRef(path->getCoordinateCount() - 4).setCurveStart(false);
	
	Q_ASSERT(!path->parts().empty());
	path->parts().front().setClosed(checked, path->getCoordinateCount() > 2);
	if (!checked)
		path->deleteCoordinate(path->getCoordinateCount() - 1, false);
	
	updateCoordsTable();
	updateDeleteCoordButton();
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::areaColorChanged()
{
	auto* symbol = static_cast<AreaSymbol*>(getCurrentElementSymbol());
	symbol->color = area_color_edit->color();
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::coordinateChanged(int row, int column)
{
	auto* object = getCurrentElementObject();
	if (!object || !midpoint_object)
		return;
	
	auto coord_index = MapCoordVector::size_type(row);
	
	if (column < 2)
	{
		auto coord = MapCoord{};
		if (object->getType() == Object::Point)
		{
			auto* point = static_cast<PointObject*>(object);
			coord = point->getCoord();
		}
		else if (object->getType() == Object::Path)
		{
			auto* path = static_cast<const PathObject*>(object);
			Q_ASSERT(coord_index < path->getCoordinateCount());
			coord = path->getCoordinate(coord_index);
		}
		
		QLocale locale;
		auto ok = false;
		auto new_value = qRound(1000 * locale.toDouble(coords_table->item(row, column)->text(), &ok));
		
		if (ok)
		{
			if (column == 0)
				coord.setNativeX(new_value);
			else
				coord.setNativeY(-new_value);
			
			if (object->getType() == Object::Point)
			{
				auto* point = static_cast<PointObject*>(object);
				point->setPosition(coord);
			}
			else if (object->getType() == Object::Path)
			{
				auto* path = static_cast<PathObject*>(object);
				path->setCoordinate(coord_index, coord);
			}
			
			map->updateAllObjectsWithSymbol(symbol);
			emit symbolEdited();
		}
		
		// Update, needed in cases of rounding and error
		const QSignalBlocker blocker(coords_table);
		coords_table->item(row, column)->setText(locale.toString((column == 0) ? coord.x() : -coord.y(), 'f', 3));
	}
	else
	{
		Q_ASSERT(object->getType() == Object::Path);
		auto* path = static_cast<PathObject*>(object);
		Q_ASSERT(coord_index < path->getCoordinateCount());
		auto coord = path->getCoordinate(coord_index);
		coord.setCurveStart(coords_table->item(row, column)->checkState() == Qt::Checked);
		path->setCoordinate(coord_index, coord);
		updateDeleteCoordButton();
		
		updateCoordsTable();
		map->updateAllObjectsWithSymbol(symbol);
		emit symbolEdited();
	}
}

void PointSymbolEditorWidget::addCoordClicked()
{
	auto* object = getCurrentElementObject();
	Q_ASSERT(object->getType() == Object::Path);
	auto* path = static_cast<PathObject*>(object);
	
	int row = (coords_table->currentRow() < 0) ? coords_table->rowCount() : (coords_table->currentRow() + 1);
	int col = qMax(0, coords_table->currentColumn());
	do
	{
		if (coords_table->currentRow() < 0)
		{
			path->addCoordinate(MapCoordVector::size_type(coords_table->rowCount()), MapCoord(0, 0));
		}
		else
		{
			auto coord_index = MapCoordVector::size_type(coords_table->currentRow());
			auto current_coord = path->getCoordinate(coord_index);
			current_coord.setCurveStart(false);
			current_coord.setHolePoint(false);
			path->addCoordinate(coord_index, current_coord);
		}
	}
	while (path->getCoordinateCount() < (path->getSymbol()->getType() == Symbol::Area ? 3 : 2));
	
	updateCoordsTable();	// NOTE: incremental updates (to the curve start boxes) would be possible but mean some implementation effort
	coords_table->setCurrentItem(coords_table->item(row, col));
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::deleteCoordClicked()
{
	int row = coords_table->currentRow();
	if (row < 0)
		return;
	
	auto* object = getCurrentElementObject();
	Q_ASSERT(object->getType() == Object::Path);
	auto* path = static_cast<PathObject*>(object);
	
	// To maintain a table-like UX, avoid high-level behavior of deleteCoordinate().
	// (1) Deleting inside a curve: Turn off curve start to retain table UX.
	auto look_back_distance = path->parts().front().isClosed() ? 2 : 3;
	for (int i = qMax(int(path->parts().front().first_index), row - look_back_distance); i <= row; ++i)
	{
		path->getCoordinateRef(MapCoordVector::size_type(i)).setCurveStart(false);
	}
	// (2) Deleting first row: Avoid removal of curve start at second row.
	if (row == 0)
	{
		// Avoid special behavior of deleteCoordinate()
		auto is_curve_start = path->getCoordinate(1).isCurveStart();
		path->getCoordinateRef(1).setCurveStart(false);
		path->deleteCoordinate(0, false);
		path->getCoordinateRef(0).setCurveStart(is_curve_start);
	}
	else
	{
		path->deleteCoordinate(MapCoordVector::size_type(row), false);
	}
	
	updateCoordsTable();	// NOTE: incremental updates (to the curve start boxes) would be possible but mean some implementation effort
	coords_table->selectRow(qMax(0, row - 1));
	updateDeleteCoordButton();
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::centerCoordsClicked()
{
	auto* object = getCurrentElementObject();
	
	if (object->getType() == Object::Point)
	{
		auto* point = static_cast<PointObject*>(object);
		point->setPosition(0, 0);
	}
	else
	{
		Q_ASSERT(object->getType() == Object::Path);
		auto* path = static_cast<PathObject*>(object);
		
		auto center = MapCoordF(0, 0);
		auto size = path->getCoordinateCount();
		auto change_size = (!path->parts().empty() && path->parts().front().isClosed()) ? (size - 1) : size;
		
		Q_ASSERT(change_size > 0);
		for (auto i = 0u; i < change_size; ++i)
			center = MapCoordF(center.x() + path->getCoordinate(i).x(), center.y() + path->getCoordinate(i).y());
		center = MapCoordF(center.x() / change_size, center.y() / change_size);
		
		path->move(MapCoord{-center});
		path->update();
	}
	
	updateCoordsTable();
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

void PointSymbolEditorWidget::updateCoordsTable()
{
	auto* object = getCurrentElementObject();
	int num_rows = 1;
	if (object->getType() == Object::Path)
	{
		auto* path = static_cast<const PathObject*>(object);
		num_rows = int(path->getCoordinateCount());
		if (num_rows > 0 && path->parts().front().isClosed())
			--num_rows;
		if (path->getSymbol()->getType() == Symbol::Line)
		{
			const QSignalBlocker blocker(line_closed_check);
			line_closed_check->setChecked(num_rows > 0 && path->parts().front().isClosed());
			line_closed_check->setEnabled(num_rows > 2 || line_closed_check->isChecked());
		}
	}
	
	coords_table->setRowCount(num_rows);
	for (int i = 0; i < num_rows; ++i)
		addCoordsRow(i);
	
	center_coords_button->setEnabled(num_rows > 0);
	updateDeleteCoordButton();
}

void PointSymbolEditorWidget::addCoordsRow(int row)
{
	coords_table->setRowHeight(row, coords_table->fontMetrics().height() + 2);
	
	const QSignalBlocker blocker(coords_table);
	
	for (int c = 0; c < 3; ++c)
	{
		if (!coords_table->item(row, c))
		{
			auto* item = new QTableWidgetItem();
			switch (c)
			{
			case 0:
			case 1:
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
				item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				break;
			default:
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			}
			coords_table->setItem(row, c, item);
		}
	}
	updateCoordsRow(row);
}

void PointSymbolEditorWidget::updateCoordsRow(int row)
{
	Q_ASSERT(element_list->currentRow() > 0);
	auto* object = getCurrentElementObject();
	auto coord_index = MapCoordVector::size_type(row);
	
	MapCoordF coordF(0, 0);
	if (object->getType() == Object::Point)
		coordF = static_cast<const PointObject*>(object)->getCoordF();
	else if (object->getType() == Object::Path)
		coordF = MapCoordF(static_cast<const PathObject*>(object)->getCoordinate(coord_index));
	
	QLocale locale;
	coords_table->item(row, 0)->setText(locale.toString(coordF.x(), 'f', 3));
	coords_table->item(row, 1)->setText(locale.toString(-coordF.y(), 'f', 3));
	
	if (object->getType() == Object::Path)
	{
		auto* path = static_cast<const PathObject*>(object);
		bool has_curve_start_box = coord_index+3 < path->getCoordinateCount()
		                           && (!path->getCoordinate(coord_index+1).isCurveStart() && !path->getCoordinate(coord_index+2).isCurveStart());
		auto is_curve_part = false;
		if ( (row > 1 && path->getCoordinate(coord_index-2).isCurveStart())
		     || (row > 0 && path->getCoordinate(coord_index-1).isCurveStart()) )
		{
			has_curve_start_box = false;
			is_curve_part = true;
		}
		auto* item = coords_table->item(row, 2);
		if (has_curve_start_box)
		{
			item->setFlags(item->flags().setFlag(Qt::ItemIsUserCheckable, true));
			item->setCheckState(path->getCoordinate(coord_index).isCurveStart() ? Qt::Checked : Qt::Unchecked);
		}
		else if (is_curve_part)
		{
			item->setFlags(item->flags().setFlag(Qt::ItemIsUserCheckable, false));
			item->setCheckState(Qt::PartiallyChecked);
		}
		else
		{
			item->setData(Qt::CheckStateRole, {});
		}
	}
}

void PointSymbolEditorWidget::updateDeleteCoordButton()
{
	auto enabled = false;
	switch (getCurrentElementObject()->getSymbol()->getType())
	{
	case Symbol::Line:
		if (!line_closed_check->isChecked())
		{
			enabled = coords_table->rowCount() > 2;
			break;
		}
		Q_FALLTHROUGH();
	case Symbol::Area:
		enabled = coords_table->rowCount() > 3;
		break;
	default:
		;
	}
	delete_coord_button->setEnabled(enabled);
}

void PointSymbolEditorWidget::insertElement(Object* object, Symbol* element_symbol)
{
	int row = (element_list->currentRow() < 0) ? element_list->count() : (element_list->currentRow() + 1);
	int pos = row - 1;
	symbol->addElement(pos, object, element_symbol);
	element_list->insertItem(row, getLabelForSymbol(element_symbol));
	element_list->setCurrentRow(row);
	map->updateAllObjectsWithSymbol(symbol);
	emit symbolEdited();
}

QString PointSymbolEditorWidget::getLabelForSymbol(const Symbol* symbol) const
{
	if (symbol->getType() == Symbol::Point)
		return tr("Point");						// FIXME: This is rather a circle.
	else if (symbol->getType() == Symbol::Line)
		return tr("Line");
	else if (symbol->getType() == Symbol::Area)
		return tr("Area");
	
	Q_ASSERT(false);
	return tr("Unknown");
}

Object* PointSymbolEditorWidget::getCurrentElementObject()
{
	if (element_list->currentRow() > 0)
		return symbol->getElementObject(element_list->currentRow() - 1);
	else
		return midpoint_object;
}

Symbol* PointSymbolEditorWidget::getCurrentElementSymbol()
{
	if (element_list->currentRow() > 0)
		return symbol->getElementSymbol(element_list->currentRow() - 1);
	else
		return symbol;
}



// ### PointSymbolEditorTool ###

PointSymbolEditorTool::PointSymbolEditorTool(MapEditorController* editor, PointSymbolEditorWidget* symbol_editor)
: MapEditorTool(editor, Other)
, symbol_editor(symbol_editor)
{
	// nothing
}

PointSymbolEditorTool::~PointSymbolEditorTool() = default;

void PointSymbolEditorTool::init()
{
	setStatusBarText(tr("<b>Click</b>: Add a coordinate. <b>%1+Click</b>: Change the selected coordinate. ").arg(ModifierKey::control()));
	
	MapEditorTool::init();
}

bool PointSymbolEditorTool::mousePressEvent(QMouseEvent* event, const MapCoordF& map_coord, MapWidget* map_widget)
{
	Q_UNUSED(map_widget);
	if (event->button() == Qt::LeftButton)
	{
		if (event->modifiers() & Qt::ControlModifier)
			symbol_editor->changeCurrentCoordinate(map_coord);
		else
			symbol_editor->addCoordinate(map_coord);
		return true;
	}
	return false;
}

const QCursor& PointSymbolEditorTool::getCursor() const
{
	static auto const cursor = scaledToScreen(QCursor{ QPixmap(QString::fromLatin1(":/images/cursor-crosshair.png")), 11, 11 });
	return cursor;
}



// ### PointSymbolEditorActivity ###

const int PointSymbolEditorActivity::cross_radius = 5;

PointSymbolEditorActivity::PointSymbolEditorActivity(Map* map, PointSymbolEditorWidget* symbol_editor)
: MapEditorActivity()
, map(map)
, symbol_editor(symbol_editor)
{
	// nothing
}

PointSymbolEditorActivity::~PointSymbolEditorActivity() = default;

void PointSymbolEditorActivity::init()
{
	update();
}

void PointSymbolEditorActivity::update()
{
	auto rect = QRectF(0.0, symbol_editor->offset_y, 0.0, 0.0);
	map->setActivityBoundingBox(rect, cross_radius + 1);
}

void PointSymbolEditorActivity::draw(QPainter* painter, MapWidget* map_widget)
{
	auto midpoint = map_widget->mapToViewport(symbol_editor->object_origin_coord).toPoint();
	
	auto pen = QPen(Qt::white);
	pen.setWidth(3);
	painter->setPen(pen);
	painter->drawLine(midpoint + QPoint(0, -cross_radius), midpoint + QPoint(0, cross_radius));
	painter->drawLine(midpoint + QPoint(-cross_radius, 0), midpoint + QPoint(cross_radius, 0));
	
	pen.setWidth(0);
	pen.setColor(Qt::black);
	painter->setPen(pen);
	painter->drawLine(midpoint + QPoint(0, -cross_radius), midpoint + QPoint(0, cross_radius));
	painter->drawLine(midpoint + QPoint(-cross_radius, 0), midpoint + QPoint(cross_radius, 0));
}


}  // namespace LibreMapper
