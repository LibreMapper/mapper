/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Matthias Kühlewein (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_MAP_NOTES_H
#define LIBREMAPPER_MAP_NOTES_H

#include <QDialog>
#include <QObject>

// IWYU pragma: no_include <QString>
class QTextEdit;
class QWidget;


namespace LibreMapper {

class Map;

class MapNotesDialog : public QDialog
{
Q_OBJECT
public:
	/**
	 * Creates a new MapNotesDialog object.
	 */
	MapNotesDialog(QWidget* parent, Map& map);
	
	~MapNotesDialog() override;
	
private slots:
	void okClicked();
	
private:
	QTextEdit* text_edit;
	
	Map& map;
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_MAP_NOTES_H
