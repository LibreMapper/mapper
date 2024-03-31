/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_UNDO_MANAGER_T_H
#define LIBREMAPPER_UNDO_MANAGER_T_H

#include <QObject>


/**
 * @test Tests UndoManager.
 */
class UndoManagerTest : public QObject
{
	Q_OBJECT
	
private slots:
	/**
	 * Performs actions on an UndoManager and observes its behaviour.
	 */
	void testUndoRedo();
	
private:
	bool clean_changed;
	bool clean;
	
	bool loaded_changed;
	bool loaded;
	
	bool can_redo_changed;
	bool can_redo;
	
	bool can_undo_changed;
	bool can_undo;
	
protected:
	/**
	 * Resets all flags named *_changed.
	 */
	void resetAllChanged();
	
protected slots:
	/**
	 * Observes UndoManager::loadedChanged(bool) signals.
	 */
	void loadedChanged(bool loaded);
	
	/**
	 * Observes UndoManager::cleanChanged(bool) signals.
	 */
	void cleanChanged(bool clean);
	
	/**
	 * Observes UndoManager::canUndoChanged(bool) signals.
	 */
	void canUndoChanged(bool can_undo);
	
	/**
	 * Observes UndoManager::canRedoChanged(bool) signals.
	 */
	void canRedoChanged(bool can_redo);
};

#endif // LIBREMAPPER_UNDO_MANAGER_T_H
