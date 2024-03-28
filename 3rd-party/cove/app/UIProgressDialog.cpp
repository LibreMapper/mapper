/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 * Copyright 2020 Kai Pastor
 *
 * This file is part of CoVe.
 */

#include "UIProgressDialog.h"

#include <QCoreApplication>

namespace cove {
//@{
//! \ingroup gui

/*! \class UIProgressDialog
 * \brief A proxy for a QProgressDialog
 * 
 * This class implements ProgressObserver and connects it to a modal
 * QProgressDialog. The dialog is closed when the proxy is destroyed
 * (or when the user clicks Cancel).
 */

/*! Constructor, essentially the same as QProgressDialog's constructor. Sets
 * progressbar range to 0-100.
 */
UIProgressDialog::UIProgressDialog(const QString& labelText,
                                   const QString& cancelButtonText,
                                   QWidget* creator, Qt::WindowFlags /*unused*/)
	: pDialog(labelText, cancelButtonText, 0, 100, creator)
{
	pDialog.setAutoClose(false);  // avoid reopening on late setValue().
	pDialog.setAutoReset(false);  // avoid jumping to zero when input is noisy.
	pDialog.setMinimumDuration(0);
	pDialog.setWindowModality(Qt::WindowModal); 
}

/*! Destructor.
 */
UIProgressDialog::~UIProgressDialog() = default;

/*! Implementation of ProgressObserver abstract method.
 */
void UIProgressDialog::setPercentage(int percentage)
{
	// QProgressDialog::setValue(int) will take care of calling
	// QApplication::processEvents() when the dialog is modal, but it
	// would make the dialog reappear if was already canceled and hidden.
	if (pDialog.wasCanceled())
		QCoreApplication::processEvents();
	else
		pDialog.setValue(percentage);
}

/*! Implementation of ProgressObserver abstract method, returns
 * value of the progress bar dialog cancel status.
 */
bool UIProgressDialog::isInterruptionRequested() const
{
	// Handle main thread events (such as UI input).
	QCoreApplication::processEvents();
	return pDialog.wasCanceled();
}

} // cove

//@}
