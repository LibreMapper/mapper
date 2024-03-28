/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_COLORSEDITFORM_H
#define COVE_COLORSEDITFORM_H

#include <vector>

#include <QAbstractTableModel>
#include <QDialog>
#include <QObject>
#include <QRgb>
#include <QString>
#include <QVariant>
#include <Qt>

#include "ui_colorseditform.h"

class QModelIndex;
class QWidget;

namespace cove {
class ColorsListModel : public QAbstractTableModel
{
	Q_OBJECT

private:
	std::vector<QRgb> colors;
	std::vector<QString> comments;
	ColorsListModel();

public:
	ColorsListModel(const std::vector<QRgb>& colors,
	                const std::vector<QString>& comments);
	int rowCount(const QModelIndex&) const override;
	int columnCount(const QModelIndex&) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
	                    int role) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	bool setData(const QModelIndex& index, const QVariant& value,
	             int role) override;

	auto getColors();
	auto getComments();
};

class ColorsEditingDelegate;

class ColorsEditForm : public QDialog
{
	Q_OBJECT

private:
	Ui::ColorsEditForm ui;
	ColorsListModel* m;
	ColorsEditingDelegate* d;

public:
	enum ColorsSource
	{
		Random = 0,
		RandomFromImage,
		Predefined
	};
	ColorsEditForm(QWidget* parent = nullptr);
	~ColorsEditForm() override;
	std::vector<QRgb> getColors();
	std::vector<QString> getComments();
	void setColors(const std::vector<QRgb>& colors,
	               const std::vector<QString>& comments);
	ColorsSource getColorsSource();
	void setColorsSource(ColorsSource s);

public slots:
	void on_predefinedColorsButton_toggled(bool checked);  // clazy:exclude=connect-by-name
};
} // cove

#endif
