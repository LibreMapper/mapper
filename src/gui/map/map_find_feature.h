/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_MAP_FIND_FEATURE_H
#define LIBREMAPPER_MAP_FIND_FEATURE_H

#include <QtGlobal>
#include <QObject>
#include <QPointer>
#include <QString>

class QAction;
class QDialog;
class QStackedLayout;
class QTextEdit;
class QWidget;

namespace OpenOrienteering {

class MapEditorController;
class ObjectQuery;
class TagSelectWidget;


/**
 * Provides an interactive feature for finding objects in the map.
 * 
 * The search text from the provided dialog is trimmed and parsed as an 
 * ObjectQuery. If parsing fails, the trimmed text is used to find any matching
 * text in tag keys, tag values, text object content or symbol name.
 */
class MapFindFeature : public QObject
{
	Q_OBJECT
	
public:
	MapFindFeature(MapEditorController& controller);
	
	~MapFindFeature() override;
	
	void setEnabled(bool enabled);
	
	QAction* showDialogAction() { return show_action; }
	
	QAction* findNextAction() { return find_next_action; }
	
private:
	void showDialog();
	
	ObjectQuery makeQuery() const;
	
	void findNext();
	
	void findAll();
	
	void showHelp() const;
	
	void tagSelectorToggled(bool active);
	
	MapEditorController& controller;
	QPointer<QDialog> find_dialog;           // child of controller's window
	QStackedLayout* editor_stack = nullptr;  // child of find_dialog
	QTextEdit* text_edit = nullptr;          // child of find_dialog
	TagSelectWidget* tag_selector = nullptr; // child of find_dialog
	QWidget* tag_selector_buttons = nullptr; // child of find_dialog
	QAction* show_action = nullptr;          // child of this
	QAction* find_next_action = nullptr;     // child of this
	
	Q_DISABLE_COPY(MapFindFeature)
};


}  // namespace OpenOrienteering

#endif
