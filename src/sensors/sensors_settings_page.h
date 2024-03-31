/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_SENSORS_SETTINGS_PAGE_H
#define LIBREMAPPER_SENSORS_SETTINGS_PAGE_H

#include <QObject>
#include <QString>

#include "gui/widgets/settings_page.h"

class QComboBox;
class QWidget;

namespace LibreMapper {


class SensorsSettingsPage : public SettingsPage
{
Q_OBJECT
public:
	explicit SensorsSettingsPage(QWidget* parent = nullptr);
	
	~SensorsSettingsPage() override;
	
	QString title() const override;

	void apply() override;
	
	void reset() override;
	
protected:
	void updateWidgets();
	
private:
	QComboBox* position_source_box = nullptr;
	QComboBox* nmea_serialport_box = nullptr;
	
};


}  // namespace LibreMapper

#endif  // LIBREMAPPER_SENSORS_SETTINGS_PAGE_H
