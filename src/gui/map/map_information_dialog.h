/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Matthias Kühlewein (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_MAP_INFORMATION_DIALOG_H
#define LIBREMAPPER_MAP_INFORMATION_DIALOG_H

#include <memory>

#include <QDialog>
#include <QObject>
#include <QString>

namespace LibreMapper {

class MainWindow;
class Map;
class MapInformation;

/**
 * A class for providing information about the current map.
 * 
 * Information is given about the number of objects in total and per map part,
 * the number of symbols, templates and undo/redo steps.
 * For each color a list of symbols using that color is shown.
 * All fonts (and their substitutions) being used by symbols are shown.
 * For objects there is a hierarchical view:
 * - the number of objects per symbol class (e.g., Point symbols, Line symbols etc.)
 *   - for each symbol class the symbols in use and the related number of objects
 *     - for each symbol the colors used by it
 */
class MapInformationDialog : public QDialog
{
Q_OBJECT
public:
	/**
	 * Creates a new MapInformationDialog object.
	 */
	MapInformationDialog(MainWindow* parent, Map* map);
	
	~MapInformationDialog() override;
	
private slots:
	void save();
	
private:
	const MainWindow* main_window;
	
	std::unique_ptr<MapInformation> map_information;
};

}  // namespace LibreMapper

#endif // LIBREMAPPER_MAP_INFORMATION_DIALOG_H
