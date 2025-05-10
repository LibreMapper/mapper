/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013-2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifdef QT_PRINTSUPPORT_LIB

#ifndef LIBREMAPPER_PRINT_PROGRESS_DIALOG_H
#define LIBREMAPPER_PRINT_PROGRESS_DIALOG_H

#include <Qt>
#include <QObject>
#include <QProgressDialog>
#include <QString>

class QPrinter;
class QWidget;

namespace LibreMapper {

class MapPrinter;


/**
 * PrintProgressDialog is a variation of QProgressDialog to be used with MapPrinter.
 * 
 * PrintProgressDialog connects to the MapPrint::printMapProgress() signal.
 * It provides a paintRequested slot which is to be connected to the
 * corresponding QPrintPreviewDialog signal.
 * 
 * This dialog is modal (for the application) by default.
 */
class PrintProgressDialog : public QProgressDialog
{
Q_OBJECT
public:
	/**
	 * Constructs a new dialog for the given MapPrinter.
	 * 
	 * map_printer must not be nullptr.
	 */
	PrintProgressDialog(MapPrinter* map_printer, QWidget* parent = nullptr, Qt::WindowFlags f = {});
	
	/**
	 * Destructor.
	 */
	~PrintProgressDialog() override;
	
public slots:
	/**
	 * Listens to and forwards paint requests.
	 * 
	 * Shows an error message if printing fails.
	 */
	void paintRequested(QPrinter* printer);
	
protected slots:
	/**
	 * Listens to printing progress messages.
	 * 
	 * Shows the dialog if it was hidden, and processes events before returning.
	 * This makes it possible to react on the dialog's Cancel button, and to
	 * draw UI updates.
	 * 
	 * @param value   The progress, from 0 (not started) to 100 (finished).
	 * @param message The text to be shown as a label to the progress.
	 */
	void setProgress(int value, const QString& message);
	
private:
	MapPrinter* const map_printer;
};


}  // namespace LibreMapper

#endif // LIBREMAPPER_PRINT_PROGRESS_DIALOG_H

#endif // QT_PRINTSUPPORT_LIB
