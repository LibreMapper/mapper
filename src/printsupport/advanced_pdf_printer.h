/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_ADVANCED_PDF_H
#define LIBREMAPPER_ADVANCED_PDF_H

#include <memory>

#include <QPaintEngine>
#include <QPrinter>


class AdvancedPdfPrintEngine;

class AdvancedPdfPrinter : public QPrinter
{
public:
	/** Creates a PdfFormat printer based on AdvancedPdfPrintEngine. */
	explicit AdvancedPdfPrinter(PrinterMode mode = ScreenResolution);
	
	/** Creates a PdfFormat printer based on AdvancedPdfPrintEngine. */
	explicit AdvancedPdfPrinter(const QPrinterInfo& printer, PrinterMode mode = ScreenResolution);
	
	/** Destructor. */
	~AdvancedPdfPrinter() override;
	
	/** Returns the paint engine type which is used for advanced pdf generation. */
	static QPaintEngine::Type paintEngineType();
	
private:
	void init();
	
	std::unique_ptr<AdvancedPdfPrintEngine> engine;
};

#endif
