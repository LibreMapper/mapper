/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "task_dialog.h"

#include <Qt>
#include <QAbstractButton>
#include <QCommandLinkButton>
#include <QDialogButtonBox>
#include <QFlags>
#include <QLabel>
#include <QSignalMapper>
#include <QVBoxLayout>

#include "util/backports.h"  // IWYU pragma: keep


namespace OpenOrienteering {

TaskDialog::TaskDialog(QWidget* parent, const QString& title, const QString& text, QDialogButtonBox::StandardButtons buttons)
 : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
	setWindowTitle(title);
	
	QLabel* text_label = nullptr;
	if (!text.isEmpty())
		text_label = new QLabel(text);
	
	button_box = nullptr;
	if (buttons != QDialogButtonBox::NoButton)
		button_box = new QDialogButtonBox(buttons);
	
	layout = new QVBoxLayout();
	if (text_label)
		layout->addWidget(text_label);
	if (button_box)
		layout->addWidget(button_box);
	setLayout(layout);
	
	signal_mapper = new QSignalMapper(this);
	connect(signal_mapper, QOverload<QWidget*>::of(&QSignalMapper::mapped), this, QOverload<QWidget*>::of(&TaskDialog::buttonClicked));
	connect(button_box, &QDialogButtonBox::clicked, this, QOverload<QWidget*>::of(&TaskDialog::buttonClicked));
}

QCommandLinkButton* TaskDialog::addCommandButton(const QString& text, const QString& description)
{
	QCommandLinkButton* button = new QCommandLinkButton(text, description);
	signal_mapper->setMapping(button, button);
	connect(button, QOverload<bool>::of(&QAbstractButton::clicked), signal_mapper, QOverload<>::of(&QSignalMapper::map));
	
	layout->insertWidget(layout->count() - (button_box ? 1 : 0), button);
	return button;
}

void TaskDialog::buttonClicked(QWidget* button)
{
	buttonClicked(static_cast<QAbstractButton*>(button));
}
void TaskDialog::buttonClicked(QAbstractButton* button)
{
	clicked_button = button;
	if (button_box->buttonRole(clicked_button) == QDialogButtonBox::RejectRole)
		reject();
	else
		accept();
}


}  // namespace OpenOrienteering
