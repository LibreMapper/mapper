/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013-2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifdef QT_PRINTSUPPORT_LIB

#include "print_progress_dialog.h"

#include <QtGlobal>
#include <QApplication>
#include <QMessageBox>

#include "core/map_printer.h"


namespace OpenOrienteering {

PrintProgressDialog::PrintProgressDialog(MapPrinter* map_printer, QWidget* parent, Qt::WindowFlags f)
 : QProgressDialog(parent, f)
 , map_printer(map_printer)
{
	setWindowModality(Qt::ApplicationModal); // Required for OSX, cf. QTBUG-40112
	setRange(0, 100);
	setMinimumDuration(0);
	setValue(0);
	
	Q_ASSERT(map_printer);
	connect(map_printer, &MapPrinter::printProgress, this, &PrintProgressDialog::setProgress);
	connect(this, &PrintProgressDialog::canceled, map_printer, &MapPrinter::cancelPrintMap);
}

PrintProgressDialog::~PrintProgressDialog()
{
	// nothing, not inlined
}

void PrintProgressDialog::paintRequested(QPrinter* printer)
{
	if (!map_printer->printMap(printer))
	{
		QMessageBox::warning(
		  parentWidget(), tr("Printing", "PrintWidget"),
		  tr("An error occurred during processing.", "PrintWidget"),
		  QMessageBox::Ok, QMessageBox::Ok );
	}
}

void PrintProgressDialog::setProgress(int value, const QString& status)
{
	setLabelText(status);
	setValue(value);
	if (!isVisible() && value < maximum())
	{
		show();
	}
	
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100 /* ms */); // Drawing and Cancel events
}


}  // namespace OpenOrienteering

#endif
