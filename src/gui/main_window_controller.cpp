/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2014 Thomas Schöps, Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "main_window_controller.h"

#include <QtGlobal>
#include <QMessageBox>

#include "settings.h"
#include "fileformats/file_format.h"
#include "fileformats/file_format_registry.h"
#include "gui/main_window.h"
#include "gui/map/map_editor.h"


namespace OpenOrienteering {

MainWindowController::~MainWindowController() = default;


bool MainWindowController::menuBarVisible()
{
	return !Settings::getInstance().touchModeEnabled();
}

bool MainWindowController::statusBarVisible()
{
	return !Settings::getInstance().touchModeEnabled();
}


bool MainWindowController::saveTo(const QString& /*path*/, const FileFormat& /*format*/)
{
	return false;
}

bool MainWindowController::exportTo(const QString& path)
{
	auto format = FileFormats.findFormatForFilename(path, &FileFormat::supportsWriting);
	if (!format)
		format = FileFormats.findFormat(FileFormats.defaultFormat());
	if (!format)
	{
		QMessageBox::warning(window,
		                     tr("Error"),
		                     tr("Cannot export the map as\n\"%1\"\n"
		                        "because the format is unknown.").arg(path));
		return false;
	}
	if (!format->supportsWriting())
	{
		QMessageBox::warning(window,
		                     tr("Error"),
		                     tr("Cannot export the map as\n\"%1\"\n"
		                        "because saving as %2 (.%3) is not supported.").
		                     arg(path, format->description(),
		                         format->fileExtensions().join(QLatin1String(", "))));
		return false;
	}
	return exportTo(path, *format);
}

bool MainWindowController::exportTo(const QString& /*path*/, const FileFormat& /*format*/)
{
	return false;
}

bool MainWindowController::loadFrom(const QString& /*path*/, const FileFormat& /*format*/, QWidget* /*dialog_parent*/)
{
	return false;
}

void MainWindowController::detach()
{
	// nothing
}

bool MainWindowController::isEditingInProgress() const
{
	return false;
}

bool MainWindowController::keyPressEventFilter(QKeyEvent* event)
{
	Q_UNUSED(event);
	return false;
}

bool MainWindowController::keyReleaseEventFilter(QKeyEvent* event)
{
	Q_UNUSED(event);
	return false;
}

MainWindowController* MainWindowController::controllerForFile(const QString& filename)
{
	if (FileFormats.findFormatForFilename(filename, &FileFormat::supportsReading))
		return new MapEditorController(MapEditorController::MapEditor);
	
	return nullptr;
}


}  // namespace OpenOrienteering
