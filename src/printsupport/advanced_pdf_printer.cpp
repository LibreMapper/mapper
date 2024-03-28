/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "advanced_pdf_printer.h"

#include <advanced_pdf_p.h>
#include <printengine_advanced_pdf_p.h>


AdvancedPdfPrinter::AdvancedPdfPrinter(QPrinter::PrinterMode mode)
 : QPrinter{ mode }
 , engine{ new AdvancedPdfPrintEngine{ mode} }
{
	init();
}

AdvancedPdfPrinter::AdvancedPdfPrinter(const QPrinterInfo& printer, QPrinter::PrinterMode mode)
 : QPrinter{ printer, mode }
 , engine{ new AdvancedPdfPrintEngine{ mode} }
{
	init();
}

AdvancedPdfPrinter::~AdvancedPdfPrinter() = default;

void AdvancedPdfPrinter::init()
{
	setOutputFormat(PdfFormat);
	setEngines(engine.get(), engine.get());
}


QPaintEngine::Type AdvancedPdfPrinter::paintEngineType()
{
	return AdvancedPdfEngine::PaintEngineType;
}
