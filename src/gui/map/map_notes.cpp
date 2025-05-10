/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012 Thomas Schöps (OpenOrienteering)
 * Copyright 2024 Matthias Kühlewein (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "map_notes.h"

#include <Qt>
#include <QtGlobal>
#include <QDialogButtonBox>
#include <QFlags>
#include <QFontMetrics>
#include <QLatin1Char>
#include <QRect>
#include <QSize>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "core/map.h"


namespace LibreMapper {

MapNotesDialog::MapNotesDialog(QWidget* parent, Map& map)
: QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
, map { map }
{
	setWindowTitle(tr("Map notes"));
	
	text_edit = new QTextEdit();
	text_edit->setPlainText(map.getMapNotes());
	auto* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	
	auto* layout = new QVBoxLayout();
	layout->addWidget(text_edit);
	layout->addWidget(button_box);
	setLayout(layout);
	
	const auto max_size = parent ? parent->window()->size() : QSize{800, 600};
	const QFontMetrics text_font_metrics(text_edit->currentFont());
	auto preferred_width = text_font_metrics.boundingRect(QString(60, QLatin1Char('m'))).width();
	const auto bounding_rect = text_font_metrics.boundingRect(0, 0, preferred_width, max_size.height(), Qt::TextWordWrap, map.getMapNotes());
	auto width = qBound(300, bounding_rect.width() + 60, max_size.width());
	auto height = qBound(200, bounding_rect.height() + 80, max_size.height());
	resize(width, height);
	
	connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(button_box, &QDialogButtonBox::accepted, this, &MapNotesDialog::okClicked);	
}

MapNotesDialog::~MapNotesDialog() = default;

// slot
void MapNotesDialog::okClicked()
{
	auto text = text_edit->toPlainText();
	if (text != map.getMapNotes())
	{
		map.setMapNotes(text);
		map.setHasUnsavedChanges(true);
	}
	
	accept();
}

}  // namespace LibreMapper
