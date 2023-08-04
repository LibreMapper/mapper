/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2021 Libor Pecháček (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_PAINT_ON_TEMPLATE_SETTINGS_PAGE_H
#define LIBREMAPPER_PAINT_ON_TEMPLATE_SETTINGS_PAGE_H

#include <functional>
#include <vector>

#include <QCheckBox>
#include <QObject>
#include <QString>

#include "settings_page.h"

class QColor;
class QLineEdit;
class QTableWidget;
class QRadioButton;
class QToolButton;
class QWidget;


namespace LibreMapper {


class PaintOnTemplateSettingsPage : public SettingsPage
{
	Q_OBJECT

public:
	explicit PaintOnTemplateSettingsPage(QWidget* parent = nullptr);
	~PaintOnTemplateSettingsPage();
	QString title() const override;
	void apply() override;
	void reset() override;

public slots:
	void editColor(bool clicked);
	void moveColorUp(bool clicked);
	void moveColorDown(bool clicked);
	void dropColor(bool clicked);
	void addColor(bool clicked);

protected:
	QTableWidget* color_table = {};
	std::vector<QRadioButton*> preset_buttons;
	QLineEdit* custom_string_edit = {};
	QToolButton* delete_button;
	QToolButton* edit_button;
	QToolButton* move_up_button;
	QToolButton* move_down_button;
	QCheckBox* use_antialiasing;

	void initializePage(const std::vector<QColor>& working_colors);
	void applyPresets();
	void updateCustomColorsString();
	std::vector<QColor> getWorkingColorsFromPage();

private:
	void moveColor(const std::function<bool (int)>& is_within_bounds, int amount);
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_PAINT_ON_TEMPLATE_SETTINGS_PAGE_H
