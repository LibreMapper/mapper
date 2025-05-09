/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "symbol_report_feature.h"

#include <algorithm>
#include <iterator>

#include <Qt>
#include <QAction>
#include <QDesktopServices>
#include <QIODevice>
#include <QLatin1String>
#include <QSaveFile>
#include <QString>
#include <QStringList>
#include <QUrl>

#include "core/symbols/html_symbol_report.h"
#include "gui/file_dialog.h"
#include "gui/main_window.h"
#include "gui/map/map_editor.h"
#include "gui/util_gui.h"

namespace LibreMapper {


namespace {

static const char* const html_extensions[2] = { ".htm", ".html" };
static const char* const html_filter        = { "*.htm *.html" };

QString htmlFileFilter()
{
	static const QString filter_template(QLatin1String("%1 (%2)"));
	QStringList filters = {
	    filter_template.arg(::LibreMapper::SymbolReportFeature::tr("HTML"), QLatin1String(html_filter)),
	    ::LibreMapper::SymbolReportFeature::tr("All files (*.*)")
	};
	return filters.join(QLatin1String(";;"));
}

QString ensureHtmlExtension(QString filepath)
{
	using std::begin; using std::end;
	auto const has_html_extension = std::any_of(begin(html_extensions), end(html_extensions), [&filepath](const auto& extension) {
		return filepath.endsWith(QString::fromLatin1(extension), Qt::CaseInsensitive);
	});
	if (!has_html_extension)
		filepath.append(QString::fromLatin1(html_extensions[0]));
	return filepath;
}

}

SymbolReportFeature::SymbolReportFeature(MapEditorController& controller)
: QObject{nullptr}
, controller{controller}
{
	show_action = new QAction(tr("Create symbol set report..."), this);
	show_action->setMenuRole(QAction::NoRole);
	show_action->setWhatsThis(Util::makeWhatThis("symbols_menu.html"));
	connect(show_action, &QAction::triggered, this, &SymbolReportFeature::showDialog);
}

SymbolReportFeature::~SymbolReportFeature() = default;

void SymbolReportFeature::setEnabled(bool enabled)
{
	show_action->setEnabled(enabled);
}

void SymbolReportFeature::showDialog()
{
	const auto* map = controller.getMap();
	auto* window = controller.getWindow();
	if (!map || !window)
		return;
	
	auto filepath = FileDialog::getSaveFileName(
	    window,
	    ::LibreMapper::MainWindow::tr("Save file"),
	    {},
	    htmlFileFilter() );
	if (filepath.isEmpty())
		return;
	
	filepath = ensureHtmlExtension(filepath);
	QSaveFile file(filepath);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		const auto report = makeHTMLSymbolReport(*map);
		file.write(report.toUtf8());
		if(file.commit())
			QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
	}
}

}
