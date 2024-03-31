/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2015, 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_COLOR_DROPDOWN_H
#define LIBREMAPPER_COLOR_DROPDOWN_H

#include <QComboBox>
#include <QObject>

class QWidget;

namespace OpenOrienteering {

class Map;
class MapColor;


/**
 * A combobox which lets the user select a map color.
 */
class ColorDropDown : public QComboBox
{
Q_OBJECT
public:
	/**
	 * Constructs a new ColorDropDown for the colors of the given map.
	 * If spot_colors_only is true, it will only display fulltone spot colors.
	 */
	ColorDropDown(const Map* map, const MapColor* initial_color = nullptr, bool spot_colors_only = false, QWidget* parent = nullptr);
	
	/** Destructor. */
	~ColorDropDown() override;
	
	/** Returns the selected color or nullptr if no color selected. */
	const MapColor* color() const;
	
	/** Sets the selection to the given color. */
	void setColor(const MapColor* color);
	
	/** Adds a color to the list. */
	void addColor(const MapColor* color);
	
	/** Updates a color in the list. */
	void updateColor(const MapColor* color);
	
	/** Removes a color from the list. */
	void removeColor(const MapColor* color);
	
protected:
	void onColorAdded(int, const MapColor* color);
	void onColorChanged(int, const MapColor* color);
	void onColorDeleted(int, const MapColor* color);
	
	const Map* map;
	const bool spot_colors_only;
};


}  // namespace OpenOrienteering

#endif
