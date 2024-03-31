/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_MEASURE_WIDGET_H
#define LIBREMAPPER_MEASURE_WIDGET_H

#include <QObject>
#include <QTextBrowser>

class QWidget;

namespace LibreMapper {

class Map;


/**
 * The widget which is shown in a dock widget when the measure tool is active.
 * Displays information about the currently selected objects.
 */
class MeasureWidget : public QTextBrowser
{
Q_OBJECT
public:
	/** Creates a new MeasureWidget for a given map. */
	MeasureWidget(Map* map, QWidget* parent = nullptr);

	/** Destroys the MeasureWidget. */
	~MeasureWidget() override;
	
protected slots:
	/**
	 * Is called when the object selection in the map changes.
	 * Updates the widget content.
	 */
	void objectSelectionChanged();
	
private:
	Map* map;
};


}  // namespace LibreMapper

#endif
