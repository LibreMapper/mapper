/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_VECTORIZATIONCONFIGFORM_H
#define COVE_VECTORIZATIONCONFIGFORM_H

#include <QDialog>
#include <QObject>
#include <QString>

#include "ui_vectorizationconfigform.h"

class QWidget;

namespace cove {
class VectorizationConfigForm : public QDialog
{
	Q_OBJECT

	Ui::VectorizationConfigForm ui;

public:
	double speckleSize, joinDistance, distDirBalance, cornerMin, optTolerance;
	bool doConnections, simpleConnectionsOnly;

	VectorizationConfigForm(QWidget* parent = nullptr);
	void accept() override;
	void setValues();

public slots:
	int exec() override;
	void on_simpleConnectionsCheckBox_toggled(bool);  // clazy:exclude=connect-by-name
	void on_doConnectionsCheckBox_toggled(bool);      // clazy:exclude=connect-by-name
};
} // cove

#endif
