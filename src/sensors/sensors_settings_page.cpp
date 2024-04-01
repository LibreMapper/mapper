/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "sensors_settings_page.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLatin1String>
#include <QList>
#include <QSpacerItem>
#include <QStringList>
#include <QVariant>

#ifdef QT_POSITIONING_LIB
#  include <QGeoPositionInfoSource>
#endif

#ifdef QT_SERIALPORT_LIB
#  include <QSerialPortInfo>
#endif

#include "settings.h"
#include "gui/util_gui.h"


namespace OpenOrienteering {

SensorsSettingsPage::SensorsSettingsPage(QWidget* parent)
: SettingsPage(parent)
{
	auto* form_layout = new QFormLayout(this);
	
#ifdef QT_POSITIONING_LIB
	form_layout->addRow(Util::Headline::create(tr("Location:")));
	
	position_source_box = new QComboBox();
	form_layout->addRow(tr("Source:"), position_source_box);
	
#ifdef QT_SERIALPORT_LIB
	auto const sources = QGeoPositionInfoSource::availableSources();
	if (sources.contains(QLatin1String("serialnmea"))
	    || sources.contains(QLatin1String("NMEA (OpenOrienteering)")))
	{
		nmea_serialport_box = new QComboBox();
		nmea_serialport_box->setEditable(true);
		form_layout->addRow(tr("Serial port (NMEA):"), nmea_serialport_box);
	}
#endif
	
	form_layout->addItem(Util::SpacerItem::create(this));
#endif
	
	updateWidgets();
}

SensorsSettingsPage::~SensorsSettingsPage() = default;


QString SensorsSettingsPage::title() const
{
	return tr("Sensors");
}


void SensorsSettingsPage::apply()
{
	auto& settings = Settings::getInstance();
	
	settings.setPositionSource(position_source_box->currentData().toString());
	if (nmea_serialport_box)
	{
		auto const port = nmea_serialport_box->currentText().trimmed();
		settings.setNmeaSerialPort(port == tr("Default") ? QString{} : port);
	}
	
	Settings::getInstance().applySettings();
}


void SensorsSettingsPage::reset()
{
	updateWidgets();
}


void SensorsSettingsPage::updateWidgets()
{
#ifdef QT_POSITIONING_LIB
	position_source_box->clear();
	
	auto const current_source_name = Settings::getInstance().positionSource();
	auto add_position_source = [this, &current_source_name](const QString& id) {
		auto display_name = id;
		if (display_name.isEmpty())
			//: Position source
			display_name = tr("Default");
		else if (display_name == QLatin1String("serialnmea"))
			//: Position source
			display_name = tr("NMEA (Qt)");
		else if (display_name == QLatin1String("NMEA (OpenOrienteering)"))
			//: Position source
			display_name = tr("NMEA (OpenOrienteering)");
		else if (display_name == QLatin1String("Windows"))
			//: Position source; product name, do not translate literally.
			display_name = tr("Windows");
		else if (display_name == QLatin1String("geoclue"))
			//: Position source; product name, do not translate literally.
			display_name = tr("GeoClue");
		else if (display_name == QLatin1String("corelocation"))
			//: Position source; product name, do not translate literally.
			display_name = tr("Core Location");
		position_source_box->addItem(display_name, id);
		if (id == current_source_name)
			position_source_box->setCurrentIndex(position_source_box->count()-1);
	};
	
	add_position_source(QString{});
	auto const position_sources = QGeoPositionInfoSource::availableSources();
	for (const auto& source : position_sources)
	{
		add_position_source(source);
	}
	
#ifdef QT_SERIALPORT_LIB
	if (nmea_serialport_box)
	{
		nmea_serialport_box->clear();
		nmea_serialport_box->addItem(tr("Default"));
		nmea_serialport_box->setCurrentIndex(0);
		
		auto const current_port_name = Settings::getInstance().nmeaSerialPort();
		auto add_serialport = [this, &current_port_name](const QString& display_name) {
			nmea_serialport_box->addItem(display_name);
			if (display_name == current_port_name)
				nmea_serialport_box->setCurrentIndex(nmea_serialport_box->count()-1);
		};
		
		auto const ports = QSerialPortInfo::availablePorts();
		for (auto const& port: ports)
		{
			add_serialport(port.portName());
		}
		if (nmea_serialport_box->currentIndex() == 0 && !current_port_name.isEmpty())
		{
			add_serialport(current_port_name);
		}
	}
#endif  // QT_SERIALPORT_LIB
#endif  // QT_POSITIONING_LIB
}


}  // namespace OpenOrienteering
