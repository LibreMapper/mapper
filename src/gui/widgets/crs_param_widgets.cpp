/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2015-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "crs_param_widgets.h"

#include <QAbstractButton>
#include <QCompleter>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStringList>
#include <QVariant>

#include "core/crs_template.h"
#include "core/crs_template_implementation.h"
#include "core/georeferencing.h"

// IWYU pragma: no_forward_declare QCompleter
// IWYU pragma: no_forward_declare QHBoxLayout
// IWYU pragma: no_forward_declare QPushButton


namespace LibreMapper {

namespace  {

QStringList makeZoneList()
{
	QStringList zone_list;
	zone_list.reserve((60 + 9) * 2);
	for (int i = 1; i <= 60; ++i)
	{
		QString zone = QString::number(i);
		zone_list << QString::fromLatin1("%1 N").arg(zone) << QString::fromLatin1("%1 S").arg(zone);
		if (i < 10)
			zone_list << QString::fromLatin1("0%1 N").arg(zone) << QString::fromLatin1("0%1 S").arg(zone);
	}
	return zone_list;
}

}  // namespace



UTMZoneEdit::UTMZoneEdit(CRSParameterWidgetObserver& observer, QWidget* parent)
 : QWidget(parent)
 , observer(observer)
{
	auto const zone_RegularExpression = QRegularExpression(QString::fromLatin1("(?:[0-5]?[1-9]|[1-6]0)(?: [NS])?"));
	auto const zone_list = makeZoneList();
	
	line_edit = new QLineEdit();
	line_edit->setValidator(new QRegularExpressionValidator(zone_RegularExpression, line_edit));
	auto* completer = new QCompleter(zone_list, line_edit);
	completer->setMaxVisibleItems(4);
	line_edit->setCompleter(completer);
	connect(line_edit, &QLineEdit::textChanged, this, &UTMZoneEdit::textChanged);
	
	auto* button = new QPushButton(tr("Calculate"));
	connect(button, &QAbstractButton::clicked, this, &UTMZoneEdit::calculateValue);
	
	auto* layout = new QHBoxLayout();
	layout->setContentsMargins({});
	layout->addWidget(line_edit, 1);
	layout->addWidget(button, 0);
	setLayout(layout);
	
	calculateValue();
}

// This non-inline definition is required to emit a (single) vtable.
UTMZoneEdit::~UTMZoneEdit() = default;


QString UTMZoneEdit::text() const
{
	return line_edit->text();
}

void UTMZoneEdit::setText(const QString& text)
{
	line_edit->setText(text);
}

bool UTMZoneEdit::calculateValue()
{
	auto georef = observer.georeferencing();
	auto zone = CRSTemplates::UTMZoneParameter::calculateUTMZone(georef.getGeographicRefPoint());
	if (!zone.isNull())
	{
		setText(zone.toString());
	}
	
	return !zone.isNull();
}


}  // namespace LibreMapper
