/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "autosave_dialog.h"

#include <QCloseEvent>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

#include "main_window.h"


namespace LibreMapper {

AutosaveDialog::AutosaveDialog(const QString& path, const QString& autosave_path, const QString& actual_path, MainWindow* parent, Qt::WindowFlags f)
: QDialog(parent, f)
, main_window(parent)
, original_path(path)
, autosave_path(autosave_path)
, resolved(false)
{
	const QString text_template = QString::fromLatin1("<b>%1</b><br/>%2<br/>%3");
	
	QFileInfo autosaved_file_info(autosave_path);
	autosaved_text.setHtml(text_template.
	   arg(tr("Autosaved file"),
	       autosaved_file_info.lastModified().toLocalTime().toString(),
	       tr("%n bytes", 0, autosaved_file_info.size())));
	
	QFileInfo user_saved_file_info(path);
	user_saved_text.setHtml(text_template.
	   arg(tr("File saved by the user"),
	       user_saved_file_info.lastModified().toLocalTime().toString(),
	       tr("%n bytes", 0, user_saved_file_info.size())) );
	
	layout = new QVBoxLayout();
	setLayout(layout);
	
	setWindowTitle(tr("File recovery"));
	
	QString intro_text = tr("File %1 was not properly closed. At the moment, there are two versions:");
	QLabel* label = new QLabel(intro_text.arg(QString::fromLatin1("<b>%1</b>").arg(user_saved_file_info.fileName())));
	label->setWordWrap(true);
	layout->addWidget(label);
	
	list_widget = new QListWidget();
	list_widget->setItemDelegate(new TextDocItemDelegate(this, this));
	list_widget->setSelectionMode(QAbstractItemView::SingleSelection);
	QListWidgetItem* item = new QListWidgetItem(list_widget, QListWidgetItem::UserType);
	item->setData(Qt::UserRole, QVariant(1));
	item = new QListWidgetItem(list_widget, QListWidgetItem::UserType);
	item->setData(Qt::UserRole, QVariant(2));
	layout->addWidget(list_widget);
	
	label = new QLabel(tr("Save the active file to remove the conflicting version."));
	label->setWordWrap(true);
	layout->addWidget(label);
	
	setSelectedPath(actual_path);
	
	connect(list_widget, &QListWidget::currentRowChanged, this, &AutosaveDialog::currentRowChanged, Qt::QueuedConnection);
	
#if defined(Q_OS_ANDROID)
	setWindowState((windowState() & ~(Qt::WindowMinimized | Qt::WindowFullScreen))
	               | Qt::WindowMaximized);
#endif
}

AutosaveDialog::~AutosaveDialog()
{
	// Nothing
}

// slot
int AutosaveDialog::exec()
{
	QDialogButtonBox button_box(QDialogButtonBox::Open | QDialogButtonBox::Cancel);
	layout->addWidget(&button_box);
	connect(&button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(&button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
	const int result = QDialog::exec();
	resolved = true;
	return result;
}

// slot
void AutosaveDialog::autosaveConflictResolved()
{
	resolved = true;
	if (!isModal())
		close();
}

void AutosaveDialog::closeEvent(QCloseEvent *event)
{
	if (main_window && !resolved)
		event->setAccepted(main_window->closeFile());
	else
		event->setAccepted(resolved);
}

QString AutosaveDialog::selectedPath() const
{
	const int row = list_widget->currentRow();
	switch (row)
	{
	case 0:
		return autosave_path;
		break;
	case 1:
		return original_path;
		break;
	case -1:
		break; // Nothing selected?
	default:
		qWarning("Undefined index");
	}
	return QString();
}

// slot
void AutosaveDialog::setSelectedPath(const QString& path)
{
	if (path == original_path)
		list_widget->setCurrentRow(1);
	else if (path == autosave_path)
		list_widget->setCurrentRow(0);
	else
		list_widget->setCurrentRow(-1);
}

void AutosaveDialog::currentRowChanged(int row)
{
	switch (row)
	{
	case 0:
		emit pathSelected(autosave_path);
		break;
	case 1:
		emit pathSelected(original_path);
		break;
	case -1:
		return; // Nothing selected?
	default:
		qWarning("Undefined index");
	}
}

const QTextDocument* AutosaveDialog::textDoc(const QModelIndex& index) const
{
	const QTextDocument* ret = nullptr;
	
	bool ok = true;
	int i = index.data(Qt::UserRole).toInt(&ok);
	if (ok)
	{
		switch (i)
		{
		case 1:
			ret = &autosaved_text;
			break;
		case 2:
			ret = &user_saved_text;
			break;
		default:
			qWarning("Undefined index");
		}
	}
	else
	{
		qWarning("Invalid data for UserRole");
	}
	
	return ret;
}


}  // namespace LibreMapper
