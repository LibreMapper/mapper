/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Jan Dalheimer (OpenOrienteering)
 * Copyright 2013-2016  Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_EDITOR_SETTINGS_PAGE_H
#define LIBREMAPPER_EDITOR_SETTINGS_PAGE_H

#include <QObject>
#include <QString>

#include "settings_page.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QWidget;


namespace LibreMapper {

class EditorSettingsPage : public SettingsPage
{
Q_OBJECT
public:
	explicit EditorSettingsPage(QWidget* parent = nullptr);
	
	~EditorSettingsPage() override;
	
	QString title() const override;

	void apply() override;
	
	void reset() override;
	
protected:
	void updateWidgets();
	
private:
	QDoubleSpinBox* button_size = nullptr;
	QSpinBox* icon_size;
	QCheckBox* antialiasing;
	QCheckBox* text_antialiasing;
	QSpinBox* tolerance;
	QSpinBox* snap_distance;
	QDoubleSpinBox* fixed_angle_stepping;
	QCheckBox* select_symbol_of_objects;
	QCheckBox* zoom_out_away_from_cursor;
	QCheckBox* draw_last_point_on_right_click;
	QCheckBox* keep_settings_of_closed_templates;
	QCheckBox* ignore_touch_input;
	
	QComboBox* edit_tool_delete_bezier_point_action;
	QComboBox* edit_tool_delete_bezier_point_action_alternative;
	
	QSpinBox* rectangle_helper_cross_radius;
	QCheckBox* rectangle_preview_line_width;
};


}  // namespace LibreMapper

#endif
