/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TEMPLATE_DIALOG_REOPEN_H
#define LIBREMAPPER_TEMPLATE_DIALOG_REOPEN_H

#include <QDialog>
#include <QListWidget>
#include <QObject>
#include <QString>

class QAbstractButton;
class QDropEvent;
class QPushButton;
class QWidget;

namespace OpenOrienteering {

class Map;


// clazy:excludeall=missing-qobject-macro


/**
 * Dialog showing a list of closed templates.
 * 
 * Offers to reopen those templates by dragging them into the list of open
 * templates shown next to them.
 */
class ReopenTemplateDialog : public QDialog
{
Q_OBJECT
public:
	ReopenTemplateDialog(QWidget* parent, Map* map, const QString& map_directory);
	
private slots:
	void updateClosedTemplateList();
	void clearClicked();
	void doAccept(QAbstractButton* button);
	
private:
	class OpenTemplateList : public QListWidget
	{
	// Q_OBJECT not possible in nested classes.
	public:
		OpenTemplateList(ReopenTemplateDialog* dialog);
        void dropEvent(QDropEvent* event) override;
	private:
		ReopenTemplateDialog* dialog;
	};
	
	QListWidget* closed_template_list;
	OpenTemplateList* open_template_list;
	QPushButton* clear_button;
	
	Map* map;
	QString map_directory;
};


}  // namespace OpenOrienteering

#endif
