/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "file_dialog.h"

#include <algorithm>
#include <iterator>

#include <QtGlobal>
#include <QStringRef>  // IWYU pragma: keep
#include <QVector>

#ifndef QTBUG_51712_QUIRK_ENABLED
#  if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(QT_TESTLIB_LIB)
#     define QTBUG_51712_QUIRK_ENABLED 1
#  else
#     define QTBUG_51712_QUIRK_ENABLED 0
#  endif
#endif

#if QTBUG_51712_QUIRK_ENABLED
#  include <memory>
#  include <private/qguiapplication_p.h>
#  include <qpa/qplatformdialoghelper.h>
#  include <qpa/qplatformtheme.h>
#  include <QByteArray>
#  include <QLatin1Char>
#  include <QMetaObject>
#  include <QStringList>
#endif


namespace
{
	constexpr int max_filter_length = 100;
	
}  // namespace



namespace LibreMapper {

bool FileDialog::needUpperCaseExtensions()
{
#if QTBUG_51712_QUIRK_ENABLED
	auto platform_theme = QGuiApplicationPrivate::platformTheme();
	if (platform_theme
		&& platform_theme->usePlatformNativeDialog(QPlatformTheme::DialogType::FileDialog))
	{
		std::unique_ptr<QPlatformDialogHelper> helper {
			platform_theme->createPlatformDialogHelper(QPlatformTheme::DialogType::FileDialog)
		};
		if (helper)
			return qstrncmp(helper->metaObject()->className(), "QGtk", 4) == 0;
	}
#endif
	return false;
}


void FileDialog::adjustParameters(QString& filter, QFileDialog::Options& options)
{
	using std::begin;
	using std::end;
	
	static const auto separator = QString::fromLatin1(";;");
	const auto filters = filter.splitRef(separator);
	
	bool has_long_filters = std::any_of(begin(filters), end(filters), [](auto&& item) {
		return item.length() > max_filter_length;
	});
	
#if QTBUG_51712_QUIRK_ENABLED
	static auto need_upper_case = needUpperCaseExtensions();
	if (need_upper_case)
	{
		QStringList new_filters;
		new_filters.reserve(filter.size());
		for (auto&& item : filters)
		{
			QString new_item;
			new_item.reserve(2 * item.length());
			auto split_0 = item.indexOf(QLatin1Char('('));
			auto split_1 = item.lastIndexOf(QLatin1Char(')'));
			new_item.append(item.left(split_1));
			new_item.append(QLatin1Char(' '));
			new_item.append(item.mid(split_0+1).toString().toUpper());
			new_filters.append(new_item);
			if (new_item.length() > max_filter_length)
				has_long_filters = true;
		}
		filter = new_filters.join(separator);
	}
#endif
	
	if (has_long_filters)
		options |= QFileDialog::HideNameFilterDetails;
}


}  // namespace LibreMapper
