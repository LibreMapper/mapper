/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2018, 2024, 2025 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "measure_widget.h"

#include <iterator>
#include <set>
// IWYU pragma: no_include <memory>

#include <QtGlobal>
#include <QBuffer>
#include <QByteArray>
#include <QFlags>
#include <QIODevice>
#include <QIcon>
#include <QLatin1Char>
#include <QLatin1String>
#include <QLocale>
#include <QPixmap>
#include <QScroller>
#include <QSize>
#include <QStyle>
#include <QWidget>

#include "core/map.h"
#include "core/objects/object.h"
#include "core/symbols/symbol.h"


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
		const auto* object = *begin(selected_objects);
		const auto* symbol = object->getSymbol();
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
			
			object->update();
			
			auto paper_length = object->asPath()->getPaperLength();
			auto real_length  = object->asPath()->getRealLength();
			
			auto paper_length_text = locale().toString(paper_length, 'f', 2);
			auto real_length_text  = locale().toString(real_length, 'f', 0);
			
			if (symbol->getContainedTypes() & Symbol::Area)
			{
				body.append(table_row.arg(tr("Boundary length:"),
				                          paper_length_text, tr("mm", "millimeters"),
				                          real_length_text, tr("m", "meters")));
				
				auto paper_area = object->asPath()->calculatePaperArea();
				auto real_area = object->asPath()->calculateRealArea();
				
				auto paper_area_text = locale().toString(paper_area, 'f', 2);
				auto real_area_text   = locale().toString(real_area, 'f', 0);
				
				body.append(table_row.arg(tr("Area:"),
				                          paper_area_text, tr("mm²", "square millimeters"),
				                          real_area_text , tr("m²", "square meters")));
				
				auto minimum_area = 0.001 * symbol->getMinimumArea();
				auto minimum_area_text = locale().toString(minimum_area, 'f', 2);
				
				if (paper_area < minimum_area && paper_area_text != minimum_area_text)
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
				
				auto minimum_length = 0.001 * symbol->getMinimumLength();
				auto minimum_length_text = locale().toString(minimum_length, 'f', 2);
				
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
