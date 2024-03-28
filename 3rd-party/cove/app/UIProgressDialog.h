/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 * Copyright 2020 Kai Pastor
 *
 * This file is part of CoVe.
 */

#ifndef COVE_PROGRESSDIALOG_H
#define COVE_PROGRESSDIALOG_H

#include <Qt>
#include <QProgressDialog>
#include <QString>

#include "libvectorizer/ProgressObserver.h"

class QWidget;

namespace cove {

class UIProgressDialog : public ProgressObserver
{
	QProgressDialog pDialog;
	
public:
	UIProgressDialog(const QString& labelText, const QString& cancelButtonText,
	                 QWidget* creator = nullptr, Qt::WindowFlags f = {});
	UIProgressDialog(const UIProgressDialog&) = delete;
	UIProgressDialog(UIProgressDialog&&) = delete;
	UIProgressDialog operator=(const UIProgressDialog&) = delete;
	UIProgressDialog operator=(UIProgressDialog&&) = delete;
	~UIProgressDialog() override;
	void setPercentage(int percentage) override;
	bool isInterruptionRequested() const override;
};

} // cove

#endif
