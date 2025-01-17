/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2013-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "settings.h"

#include <initializer_list>

#include <QtGlobal>
#if defined(QT_WIDGETS_LIB)
#include <QApplication>
#endif
#include <QByteArray>
#include <QColor>
#include <QGuiApplication>
#include <QLatin1Char>
#include <QLatin1String>
#include <QLocale>
#include <QRgb>
#include <QScreen>
#include <QSettings>
#include <QStringList>
#include <QStringView>
#include <QVector>


namespace LibreMapper {

/*
 * Implementation for some functions which need a Settings instance.
 * 
 * Settings has Qt dependencies only. The Util function are declared in
 * gui/util_gui.h. By moving the definitions here, we facilitate the
 * compilation of unit tests which depend on Settings.
 */
namespace Util
{

	qreal mmToPixelPhysical(qreal millimeters)
	{
		auto ppi = Settings::getInstance().getSettingCached(Settings::General_PixelsPerInch).toReal();
		return millimeters * ppi / 25.4;
	}
	
	qreal pixelToMMPhysical(qreal pixels)
	{
		auto ppi = Settings::getInstance().getSettingCached(Settings::General_PixelsPerInch).toReal();
		return pixels * 25.4 / ppi;
	}

	
	qreal mmToPixelLogical(qreal millimeters)
	{
		auto ppi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
		return millimeters * ppi / qreal(25.4);
	}
	
	qreal pixelToMMLogical(qreal pixels)
	{
		auto ppi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
		return pixels * qreal(25.4) / ppi;
	}
	
	
	bool isAntialiasingRequired(qreal ppi)
	{
		return ppi < 200;
	}
	
	bool isAntialiasingRequired()
	{
		return isAntialiasingRequired(Settings::getInstance().getSettingCached(Settings::General_PixelsPerInch).toReal());
	}
	
}



Settings& Settings::getInstance()
{
	static Settings instance;
	return instance;
}



Settings::Settings()
 : QObject()
{
	const float touch_button_minimum_size_default = 6.5;
	float symbol_widget_icon_size_mm_default;
	float map_editor_click_tolerance_default;
	float map_editor_snap_distance_default;
	int start_drag_distance_default;
	
	// Platform-specific settings defaults
#if defined(ANDROID) || !defined(QT_WIDGETS_LIB)
	symbol_widget_icon_size_mm_default = touch_button_minimum_size_default;
	map_editor_click_tolerance_default = 4.0f;
	map_editor_snap_distance_default = 15.0f;
	start_drag_distance_default = Util::mmToPixelLogical(3.0f);
#else
	symbol_widget_icon_size_mm_default = 8;
	map_editor_click_tolerance_default = 3.0f;
	map_editor_snap_distance_default = 10.0f;
	start_drag_distance_default = QApplication::startDragDistance();
#endif
	
	qreal ppi = QGuiApplication::primaryScreen()->physicalDotsPerInch();
	// Beware of https://bugreports.qt-project.org/browse/QTBUG-35701
	if (ppi > 2048.0 || ppi < 16.0)
		ppi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
	
	registerSetting(MapDisplay_TextAntialiasing, "MapDisplay/text_antialiasing", false);
	registerSetting(MapEditor_ClickToleranceMM, "MapEditor/click_tolerance_mm", map_editor_click_tolerance_default);
	registerSetting(MapEditor_SnapDistanceMM, "MapEditor/snap_distance_mm", map_editor_snap_distance_default);
	registerSetting(MapEditor_FixedAngleStepping, "MapEditor/fixed_angle_stepping", 15);
	registerSetting(MapEditor_ChangeSymbolWhenSelecting, "MapEditor/change_symbol_when_selecting", true);
	registerSetting(MapEditor_ZoomOutAwayFromCursor, "MapEditor/zoom_out_away_from_cursor", true);
	registerSetting(MapEditor_DrawLastPointOnRightClick, "MapEditor/draw_last_point_on_right_click", true);
	registerSetting(MapEditor_IgnoreTouchInput, "MapEditor/ignore_touch_input", false);
	registerSetting(MapGeoreferencing_ControlScaleFactor, "MapGeoreferencing/control_scale_factor", false);
	registerSetting(MapGeoreferencing_MagCalcServiceKey, "MapGeoreferencing/magcalc_service_access_key", QLatin1String("zNEw7")); // Assigned by NOAA NCEI
	
	registerSetting(EditTool_DeleteBezierPointAction, "EditTool/delete_bezier_point_action", int(DeleteBezierPoint_RetainExistingShape));
	registerSetting(EditTool_DeleteBezierPointActionAlternative, "EditTool/delete_bezier_point_action_alternative", int(DeleteBezierPoint_ResetHandles));
	
	registerSetting(RectangleTool_HelperCrossRadiusMM, "RectangleTool/helper_cross_radius_mm", 100.0f);
	registerSetting(RectangleTool_PreviewLineWidth, "RectangleTool/preview_line_with", true);
	
	registerSetting(Templates_KeepSettingsOfClosed, "Templates/keep_settings_of_closed_templates", true);
	
	registerSetting(ActionGridBar_ButtonSizeMM, "ActionGridBar/button_size_mm", touch_button_minimum_size_default);
	registerSetting(SymbolWidget_IconSizeMM, "SymbolWidget/icon_size_mm", symbol_widget_icon_size_mm_default);
	registerSetting(SymbolWidget_ShowCustomIcons, "SymbolWidget/show_custom_icons", true);
	
	registerSetting(General_RetainCompatiblity, "retainCompatiblity", false);
	registerSetting(General_SaveUndoRedo, "saveUndoRedo", true);
	registerSetting(General_AutosaveInterval, "autosave", 15); // unit: minutes
	registerSetting(General_Language, "language", QLocale::system().name().left(2));
	registerSetting(General_PixelsPerInch, "pixelsPerInch", ppi);
	registerSetting(General_TranslationFile, "translationFile", QVariant(QString{}));
	registerSetting(General_RecentFilesList, "recentFileList", QVariant(QStringList()));
	registerSetting(General_OpenMRUFile, "openMRUFile", false);
	registerSetting(General_Local8BitEncoding, "local_8bit_encoding", QLatin1String("Default"));
	registerSetting(General_StartDragDistance, "startDragDistance", start_drag_distance_default);
	
	registerSetting(HomeScreen_TipsVisible, "HomeScreen/tipsVisible", true);
	registerSetting(HomeScreen_CurrentTip, "HomeScreen/currentTip", -1);
	
	// Set antialiasing default depending on screen pixels per inch
	registerSetting(MapDisplay_Antialiasing, "MapDisplay/antialiasing", Util::isAntialiasingRequired(getSetting(General_PixelsPerInch).toReal()));
	
	// Paint On Template tool settings
	registerSetting(PaintOnTemplateTool_Colors, "PaintOnTemplateTool/colors", QLatin1String("FF0000,FFFF00,00FF00,DB00D9,0000FF,D15C00,000000"));

	// OCD file format compatibility settings
	registerSetting(OcdCompatLeavePathsOpenOnImport, "OcdCompatibility/leavePathsOpenOnImport", false);

	QSettings settings;
	
#ifndef Q_OS_ANDROID
	// Overwrite default value with actual setting
	touch_mode_enabled = mobileModeEnforced() || settings.value(QLatin1String("General/touch_mode_enabled"), touch_mode_enabled).toBool();
#endif
	
	sensors.position_source = settings.value(QLatin1String("Sensors/position_source"), sensors.position_source).toString();
	sensors.nmea_serialport = settings.value(QLatin1String("Sensors/nmea_serialport"), sensors.nmea_serialport).toString();
	
	// Migrate old settings
	static bool migration_checked = false;
	if (!migration_checked)
	{
		QVariant current_version { QLatin1String("0.8") };
		auto settings_version = settings.value(QLatin1String("version")).toString();
		if (!settings_version.isEmpty() && settings_version != current_version)
		{
			migrateSettings(settings, current_version);
		}
		settings.setValue(QLatin1String("version"), current_version);
		migration_checked = true;
	}
}

void Settings::registerSetting(Settings::SettingsEnum id, const char* path_latin1, const QVariant& default_value)
{
	setting_paths[id] = QString::fromLatin1(path_latin1);
	setting_defaults[id] = default_value;
}

void Settings::migrateSettings(QSettings& settings, const QVariant& version)
{
	migrateValue("General/language", General_Language, settings);
	if (migrateValue("MapEditor/click_tolerance", MapEditor_ClickToleranceMM, settings))
		settings.setValue(getSettingPath(MapEditor_ClickToleranceMM), Util::pixelToMMLogical(settings.value(getSettingPath(MapEditor_ClickToleranceMM)).toReal()));
	if (migrateValue("MapEditor/snap_distance", MapEditor_SnapDistanceMM, settings))
		settings.setValue(getSettingPath(MapEditor_SnapDistanceMM), Util::pixelToMMLogical(settings.value(getSettingPath(MapEditor_SnapDistanceMM)).toReal()));
	if (migrateValue("RectangleTool/helper_cross_radius", RectangleTool_HelperCrossRadiusMM, settings))
		settings.setValue(getSettingPath(RectangleTool_HelperCrossRadiusMM), Util::pixelToMMLogical(settings.value(getSettingPath(RectangleTool_HelperCrossRadiusMM)).toReal()));
	
	if (!settings.value(QLatin1String("MapEditor/units_rectified"), false).toBool())
	{
		const auto factor = QGuiApplication::primaryScreen()->logicalDotsPerInch()
		                    / getSetting(Settings::General_PixelsPerInch).toReal();
		for (auto setting : { SymbolWidget_IconSizeMM, MapEditor_ClickToleranceMM, MapEditor_SnapDistanceMM, RectangleTool_HelperCrossRadiusMM })
		{
			const auto path = getSettingPath(setting);
			const auto value = settings.value(path, getDefaultValue(setting)).toReal();
			settings.setValue(path, qRound(value * factor));
		}
		settings.setValue(QLatin1String("MapEditor/units_rectified"), true);
	}
	
	bool have_language_number;
	auto language_number = getSetting(General_Language).toInt(&have_language_number);
	if (have_language_number)
	{
		// Migrate old numeric language setting
		auto language = QLocale(QLocale::Language(language_number)).name().left(2);
		setSetting(Settings::General_Language, language);
	}
	
	if (!settings.value(QLatin1String("new_ocd8_implementation_v0.6")).isNull())
	{
		// Remove/reset to default
		settings.remove(QLatin1String("new_ocd8_implementation_v0.6"));
		settings.remove(QLatin1String("new_ocd8_implementation"));
	}
	
	if (!version.toByteArray().startsWith("0."))
	{
		// Future cleanup
		settings.remove(QLatin1String("new_ocd8_implementation"));
		settings.remove(QLatin1String("MapEditor/units_rectified"));
	}
}

bool Settings::migrateValue(const char* old_key_latin1, SettingsEnum new_setting, QSettings& settings) const
{
	auto old_key = QString::fromLatin1(old_key_latin1);
	bool value_migrated = false;
	if (settings.contains(old_key))
	{
		const QString new_key = getSettingPath(new_setting);
		Q_ASSERT_X(new_key != old_key, "Settings::migrateValue",
		  qPrintable(QString::fromLatin1("New key \"%1\" equals old key").arg(new_key)) );
		
		if (!settings.contains(new_key))
		{
			settings.setValue(new_key, settings.value(old_key));
			value_migrated = true;
		}
		settings.remove(old_key);
	}
	return value_migrated;
}

QVariant Settings::getSetting(Settings::SettingsEnum setting) const
{
	QSettings settings;
	return settings.value(getSettingPath(setting), getDefaultValue(setting));
}

QVariant Settings::getSettingCached(Settings::SettingsEnum setting)
{
	if (settings_cache.contains(setting))
		return settings_cache.value(setting);
	
	QSettings settings;
	QVariant value = settings.value(getSettingPath(setting), getDefaultValue(setting));
	settings_cache.insert(setting, value);
	return value;
}

void Settings::setSettingInCache(Settings::SettingsEnum setting, const QVariant& value)
{
	settings_cache.insert(setting, value);
}

void Settings::setSetting(Settings::SettingsEnum setting, const QVariant& value)
{
	bool setting_changed = true;
	if (settings_cache.contains(setting))
		setting_changed = settings_cache.value(setting) != value;
	else
		setting_changed = QSettings().value(getSettingPath(setting), getDefaultValue(setting)) != value;
	
	if (setting_changed)
	{
		QSettings().setValue(getSettingPath(setting), value);
		settings_cache.clear();
		emit settingsChanged();
	}
}

void Settings::remove(Settings::SettingsEnum setting)
{
	QVariant value = getDefaultValue(setting);
	bool setting_changed = true;
	if (settings_cache.contains(setting))
		setting_changed = settings_cache.value(setting) != value;
	else
		setting_changed = QSettings().value(getSettingPath(setting), value) != value;
	
	QSettings().remove(getSettingPath(setting));
	if (setting_changed)
	{
		settings_cache.clear();
		emit settingsChanged();
	}
}

void Settings::applySettings()
{
	QSettings().sync();
	
	// Invalidate cache as settings could be changed
	settings_cache.clear();
	emit settingsChanged();
}

int Settings::getSymbolWidgetIconSizePx()
{
	return qRound(Util::mmToPixelPhysical(getSettingCached(Settings::SymbolWidget_IconSizeMM).toReal()));
}

qreal Settings::getMapEditorClickTolerancePx()
{
	return Util::mmToPixelPhysical(getSettingCached(Settings::MapEditor_ClickToleranceMM).toReal());
}

qreal Settings::getMapEditorSnapDistancePx()
{
	return Util::mmToPixelPhysical(getSettingCached(Settings::MapEditor_SnapDistanceMM).toReal());
}

qreal Settings::getRectangleToolHelperCrossRadiusPx()
{
	return Util::mmToPixelPhysical(getSettingCached(Settings::RectangleTool_HelperCrossRadiusMM).toReal());
}

int Settings::getStartDragDistancePx()
{
	return getSettingCached(Settings::General_StartDragDistance).toInt();
}


#ifndef Q_OS_ANDROID

void Settings::setTouchModeEnabled(bool enabled)
{
	if (!mobileModeEnforced() && touch_mode_enabled != enabled)
	{
		touch_mode_enabled = enabled;
		QSettings().setValue(QLatin1String("General/touch_mode_enabled"), enabled);
		emit settingsChanged();
	}
}

// static
bool Settings::mobileModeEnforced() noexcept
{
	static bool const mobile_mode_enforced = qEnvironmentVariableIsSet("MAPPER_MOBILE_GUI")
	                                         ? (qgetenv("MAPPER_MOBILE_GUI") != "0")
	                                         : false;
	return mobile_mode_enforced;
}

#endif


void Settings::setPositionSource(const QString& name)
{
	if (name != sensors.position_source)
	{
		sensors.position_source = name;
		QSettings().setValue(QLatin1String("Sensors/position_source"), name);
		emit settingsChanged();
	}
}

void Settings::setNmeaSerialPort(const QString& name)
{
	if (name != sensors.nmea_serialport)
	{
		sensors.nmea_serialport = name;
		QSettings().setValue(QLatin1String("Sensors/nmea_serialport"), name);
		emit settingsChanged();
	}
}

std::vector<QColor> Settings::colorsStringToVector(QString config_string)
{
	auto const color_strings = QStringView{config_string}.split(QLatin1Char(','));

	auto colors = std::vector<QColor>();
	colors.reserve(color_strings.size());

	// we don't bother about malformed strings and accept the resulting zero
	for (const auto& color_string : color_strings)
		colors.emplace_back(QRgb(color_string.toUInt(nullptr, 16)));

	return colors;
}

std::vector<QColor> Settings::paintOnTemplateColors() const
{
	return colorsStringToVector(getSetting(PaintOnTemplateTool_Colors).toString());
}

QString Settings::colorsVectorToString(const std::vector<QColor>& new_colors)
{
	auto strings = QStringList {};
	strings.reserve(new_colors.size());

	for (auto const& color : new_colors)
		strings.append(color.name().right(6).toUpper());

	return strings.join(QLatin1Char(','));
}

void Settings::setPaintOnTemplateColors(const std::vector<QColor>& new_colors)
{
	setSetting(PaintOnTemplateTool_Colors, colorsVectorToString(new_colors));
}


}  // namespace LibreMapper
