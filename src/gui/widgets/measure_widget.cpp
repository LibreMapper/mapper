/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2015 Kai Pastor (OpenOrienteering)
 * Copyright 2024 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */


#include "measure_widget.h"

#include <queue>

#include <QBuffer>
#include <QLocale>
#include <QScroller>
#include <QStyle>

#include "core/map.h"
#include "core/objects/object.h"
#include "core/symbols/symbol.h"
#include "core/symbols/area_symbol.h"
#include "core/symbols/combined_symbol.h"
#include "core/symbols/line_symbol.h"


namespace LibreMapper {

MeasureWidget::MeasureWidget(Map* map, QWidget* parent)
: QTextBrowser(parent)
, map(map)
{
	auto const std_icon = style()->standardIcon(QStyle::SP_MessageBoxWarning);
	auto const pixmap = std_icon.pixmap(QSize {22, 22});
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");
	warning_icon = QLatin1String("<img align=\"right\" src=\"data:image/png;base64,")
	               + QString::fromLatin1(buffer.data().toBase64())
	               + QLatin1String("\"/>");
	
	QScroller::grabGesture(viewport(), QScroller::TouchGesture);
	
	connect(map, &Map::objectSelectionChanged, this, &MeasureWidget::objectSelectionChanged);
	connect(map, &Map::selectedObjectEdited, this, &MeasureWidget::objectSelectionChanged);
	connect(map, &Map::symbolChanged, this, &MeasureWidget::objectSelectionChanged);
	
	objectSelectionChanged();
}

MeasureWidget::~MeasureWidget() = default;


void MeasureWidget::objectSelectionChanged()
{
	QString headline;   // inline HTML
	QString body;       // HTML blocks
	QString extra_text; // inline HTML
	bool show_warning = false;
	
	auto& selected_objects = map->selectedObjects();
	if (selected_objects.empty())
	{
		extra_text = tr("No object selected.");
	}
	else if (selected_objects.size() > 1)
	{
		extra_text = tr("%1 objects selected.").arg(locale().toString(map->getNumSelectedObjects()));
	}
	else
	{
		const Object* object = *begin(selected_objects);
		const Symbol* symbol = object->getSymbol();
		headline = symbol->getNumberAsString() + QLatin1Char(' ') + symbol->getName();
		
		if (object->getType() != Object::Path)
		{
			extra_text = tr("The selected object is not a path.");
		}
		else
		{
			body = QLatin1String{ "<table>" };
			static const QString table_row{ QLatin1String{
			  "<tr><td>%1</td><td align=\"center\">%2 %3</td><td align=\"center\">(%4 %5)</td></tr>" 
			} };
			
			double paper_to_real = 0.001 * map->getScaleDenominator();
			
			object->update();
			const PathPartVector& parts = static_cast<const PathObject*>(object)->parts();
			Q_ASSERT(!parts.empty());
			
			auto paper_length = parts.front().length();
			auto real_length  = paper_length * paper_to_real;
			
			auto paper_length_text = locale().toString(paper_length, 'f', 2);
			auto real_length_text  = locale().toString(real_length, 'f', 0);
			
			if (symbol->getContainedTypes() & Symbol::Area)
			{
				body.append(table_row.arg(tr("Boundary length:"),
				                          paper_length_text, tr("mm", "millimeters"),
				                          real_length_text, tr("m", "meters")));
				
				auto paper_area = parts.front().calculateArea();
				if (parts.size() > 1)
				{
					paper_area *= 2;
					for (const auto& part : parts)
						paper_area -= part.calculateArea();
				}
				double real_area = paper_area * paper_to_real * paper_to_real;
				
				auto paper_area_text = locale().toString(paper_area, 'f', 2);
				auto real_area_text   = locale().toString(real_area, 'f', 0);
				
				body.append(table_row.arg(tr("Area:"),
				                          paper_area_text, tr("mm²", "square millimeters"),
				                          real_area_text , tr("m²", "square meters")));
				
				auto minimum_area = std::numeric_limits<double>::infinity();
				auto minimum_area_text = QString{ };
				if (symbol->getType() == Symbol::Area)
				{
					minimum_area      = 0.001 * static_cast<const AreaSymbol*>(symbol)->getMinimumArea();
					minimum_area_text = locale().toString(minimum_area, 'f', 2);
				}
				else if (symbol->getType() == Symbol::Combined)
				{
					auto to_be_examined = std::queue<CombinedSymbol const*> {};
					to_be_examined.push(symbol->asCombined());
					do
					{
						auto const* symbol = to_be_examined.front();
						to_be_examined.pop();
						for (auto part_num = 0; part_num < symbol->getNumParts(); ++part_num)
						{
							auto const* part = symbol->getPart(part_num);
							
							if (!part)
								continue;
							
							if (part->getType() == Symbol::Area)
							{
								auto symbol_min_area = 0.001 * part->asArea()->getMinimumArea();
								if (symbol_min_area < minimum_area)
									minimum_area = symbol_min_area;
							}
							else if (part->getType() == Symbol::Combined)
							{
								to_be_examined.push(part->asCombined());
							}
						}
					}
					while (!to_be_examined.empty());
					minimum_area_text = locale().toString(minimum_area, 'f', 2);
				}
				
				if (std::isfinite(minimum_area) && paper_area < minimum_area
				    && paper_area_text != minimum_area_text)
				{
					extra_text = QLatin1String("<b>") + tr("This object is too small.") + QLatin1String("</b><br/>")
					             + tr("The minimimum area is %1 %2.").arg(minimum_area_text, tr("mm²"))
					             + QLatin1String("<br/>");
					show_warning = true;
				}
				extra_text.append(tr("Note: Boundary length and area are correct only if there are no self-intersections and holes are used as such."));
			}
			else
			{
				body.append(table_row.arg(tr("Length:"),
				                          paper_length_text, tr("mm", "millimeters"),
				                          real_length_text, tr("m", "meters")));
				
				auto minimum_length  = 0.0;
				auto minimum_length_text = QString{ };
				if (symbol->getType() == Symbol::Line)
				{
					minimum_length      = 0.001 * static_cast<const LineSymbol*>(symbol)->getMinimumLength();
					minimum_length_text = locale().toString(minimum_length, 'f', 2);
				}
				
				if (paper_length < minimum_length && paper_length_text != minimum_length_text)
				{
					extra_text = QLatin1String("<b>") + tr("This line is too short.") + QLatin1String("</b><br/>")
					             + tr("The minimum length is %1 %2.").arg(minimum_length_text, tr("mm"));
					show_warning = true;
				}
			}
			
			body.append(QLatin1String("</table>"));
		}
	}
	
	if (!extra_text.isEmpty())
		body.append(QLatin1String("<p>") + extra_text + QLatin1String("</p>"));
	setHtml(QLatin1String("<p><b>") + (show_warning ? warning_icon : QLatin1String()) + headline + QLatin1String("</b></p>") + body);
}


}  // namespace LibreMapper
