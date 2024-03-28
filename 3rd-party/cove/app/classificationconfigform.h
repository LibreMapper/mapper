/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_CLASSIFICATIONCONFIGFORM_H
#define COVE_CLASSIFICATIONCONFIGFORM_H

#include <QDialog>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QObject>
#include <QString>
#include <QValidator>

#include "ui_classificationconfigform.h"

class QWidget;

namespace cove {
namespace classificationconfigform_private {
class QDoubleInfValidator : public QDoubleValidator
{
	Q_OBJECT

public:
	QDoubleInfValidator(QObject* parent);
	QDoubleInfValidator(double bottom, double top, int decimals,
	                    QObject* parent);
	QValidator::State validate(QString& input, int& pos) const override;
};
} // classificationconfigform_private

class ClassificationConfigForm : public QDialog
{
	Q_OBJECT

	const QIntValidator positiveIntegerValid;
	const QDoubleValidator double01Valid;
	const classificationconfigform_private::QDoubleInfValidator double1InfValid;
	Ui::ClassificationConfigForm ui;

public:
	double initAlpha;
	double minAlpha;
	double Q;
	int E;
	int learnMethod;
	int colorSpace;
	double p;

	ClassificationConfigForm(QWidget* parent = nullptr);
	void accept() override;
	void setValues();

public slots:
	void on_learningMethodComboBox_activated(int);  // clazy:exclude=connect-by-name
};
} // cove

#endif
