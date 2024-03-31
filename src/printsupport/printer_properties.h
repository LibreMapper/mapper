/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_PRINTER_PROPERTIES_H
#define LIBREMAPPER_PRINTER_PROPERTIES_H

#include <memory>

class QPrinter;
class QWidget;


/**
 * Provides platform-dependent printer properties handling.
 * 
 * At moment, a specific implementation exists for Win32, dealing with DEVMODE
 * and DocumentProperties.
 */
namespace PlatformPrinterProperties
{
	/**
	 * Saves the printer's platform-dependent properties.
	 * 
	 * The buffer is reset to point to the saved data.
	 * 
	 * The default implementation does nothing.
	 */
	void save(const QPrinter* printer, std::shared_ptr<void>& buffer);
	
	/**
	 * Applies platform-dependent properties to the printer, if possible.
	 * 
	 * The default implementation does nothing.
	 */
	void restore(QPrinter* printer, const std::shared_ptr<void>& buffer);
	
	/**
	 * Returns true iff the platform supports execDialog().
	 * 
	 * The default implementation returns false.
	 */
	bool dialogSupported();
	
	/**
	 * Shows a modal properties dialog for the given printer.
	 * 
	 * The printer parameter must not be nullptr. The buffer may return data
	 * which has to live as long as the printer object.
	 * 
	 * Returns QDialog::Accepted or QDialog::Rejected, depending on user action.
	 * 
	 * The default implementation returns QDialog::Rejected.
	 */
	int execDialog(QPrinter* printer, std::shared_ptr<void>& buffer, QWidget* parent = nullptr);
}

#endif // LIBREMAPPER_PRINTER_PROPERTIES_H
