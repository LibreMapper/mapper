/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_SYMBOL_REPORT_FEATURE_H
#define LIBREMAPPER_SYMBOL_REPORT_FEATURE_H

#include <QtGlobal>
#include <QObject>
// IWYU pragma: no_include <QString>

class QAction;

namespace LibreMapper {

class MapEditorController;

/**
 * Provides a UI feature for generating symbol set reports.
 */
class SymbolReportFeature : public QObject
{
	Q_OBJECT
	
public:
	SymbolReportFeature(MapEditorController& controller);
	
	~SymbolReportFeature() override;
	
	void setEnabled(bool enabled);
	
	QAction* showDialogAction() { return show_action; }
	
private:
	void showDialog();
	
	MapEditorController& controller;
	QAction* show_action;
	
	Q_DISABLE_COPY(SymbolReportFeature)
};
	
}  // namespace LibreMapper

#endif  // LIBREMAPPER_SYMBOL_REPORT_FEATURE_H
