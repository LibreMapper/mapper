/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#include "vectorizationconfigform.h"

#include <climits>
#include <limits>

#include <QCheckBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QLineEdit>
#include <Qt>

#include "ui_vectorizationconfigform.h"

class QWidget;

namespace cove {
VectorizationConfigForm::VectorizationConfigForm(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.shortPathLineEdit->setValidator(new QIntValidator(2, INT_MAX, this));
	ui.joinDistanceLineEdit->setValidator(
		new QDoubleValidator(1, std::numeric_limits<double>::infinity(), 1, this));
	ui.distDirBalanceLineEdit->setValidator(
		new QDoubleValidator(0, 1, 2, this));
}

void VectorizationConfigForm::accept()
{
	on_simpleConnectionsCheckBox_toggled(simpleConnectionsOnly);
	on_doConnectionsCheckBox_toggled(doConnections);
	speckleSize = ui.shortPathLineEdit->text().toDouble();
	doConnections = ui.doConnectionsCheckBox->checkState() == Qt::Checked;
	joinDistance = ui.joinDistanceLineEdit->text().toDouble();
	simpleConnectionsOnly =
		ui.simpleConnectionsCheckBox->checkState() == Qt::Checked;
	distDirBalance = ui.distDirBalanceLineEdit->text().toDouble();
	QDialog::accept();
}

int VectorizationConfigForm::exec()
{
	on_simpleConnectionsCheckBox_toggled(simpleConnectionsOnly);
	on_doConnectionsCheckBox_toggled(doConnections);
	ui.shortPathLineEdit->setText(QString::number(speckleSize));
	ui.doConnectionsCheckBox->setCheckState(doConnections ? Qt::Checked
														  : Qt::Unchecked);
	ui.joinDistanceLineEdit->setText(QString::number(joinDistance));
	ui.simpleConnectionsCheckBox->setCheckState(
		simpleConnectionsOnly ? Qt::Checked : Qt::Unchecked);
	ui.distDirBalanceLineEdit->setText(QString::number(distDirBalance));

	return QDialog::exec();
}

void VectorizationConfigForm::on_doConnectionsCheckBox_toggled(bool state)
{
	ui.joinDistanceLineEdit->setEnabled(state);
	ui.simpleConnectionsCheckBox->setEnabled(state);

	if (state)
		ui.distDirBalanceLineEdit->setEnabled(
			ui.simpleConnectionsCheckBox->checkState() != Qt::Checked);
	else
		ui.distDirBalanceLineEdit->setEnabled(false);
}

void VectorizationConfigForm::on_simpleConnectionsCheckBox_toggled(bool state)
{
	ui.distDirBalanceLineEdit->setEnabled(!state);
}
} // cove
