/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "printer_properties.h"

#include <QDialog>


namespace PlatformPrinterProperties
{
	void save(const QPrinter*, std::shared_ptr<void>&)
	{
		// nothing
	}
	
	void restore(QPrinter*, const std::shared_ptr<void>&)
	{
		// nothing
	}
	
	bool dialogSupported()
	{
		return false;
	}
	
	int execDialog(QPrinter*, std::shared_ptr<void>&, QWidget*)
	{
		return QDialog::Rejected;
	}
	
}
