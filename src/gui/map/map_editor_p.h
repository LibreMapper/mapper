/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013, 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2013, 2014, 2015, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_MAP_EDITOR_P_H
#define LIBREMAPPER_MAP_EDITOR_P_H

#include <QAction>
#include <QDockWidget>

class QEvent;
class QIcon;
class QObject;
class QResizeEvent;
class QSizeGrip; // IWYU pragma: keep
class QString;
class QWidget;

namespace LibreMapper {

class MapEditorController;
class Template;


/**
 * Custom QDockWidget which unchecks the associated menu action when closed
 * and delivers a notification to its child
 */
class EditorDockWidget : public QDockWidget
{
Q_OBJECT
public:
	EditorDockWidget(const QString& title, QAction* action,
					 MapEditorController* editor, QWidget* parent = nullptr);
	
protected:
	bool event(QEvent* event) override;
	void resizeEvent(QResizeEvent *event) override;
	
private:
	QAction* action;
	MapEditorController* editor;
	
#ifdef Q_OS_ANDROID
	QSizeGrip* size_grip;
#endif
};



/**
 * Helper class which disallows deselecting the checkable action by the user
 */
class MapEditorToolAction : public QAction
{
Q_OBJECT
public:
	MapEditorToolAction(const QIcon& icon, const QString& text, QObject* parent);
	
signals:
	void activated();
	
private slots:
	void triggeredImpl(bool checked);
};


}  // namespace LibreMapper

#endif
