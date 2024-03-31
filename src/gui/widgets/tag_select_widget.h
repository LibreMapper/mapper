/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Mitchell Krome (OpenOrienteering)
 * Copyright 2017-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TAG_SELECT_WIDGET_H
#define LIBREMAPPER_TAG_SELECT_WIDGET_H

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QTableWidget>

class QShowEvent;
class QToolButton;
class QWidget;

namespace OpenOrienteering {

class ObjectQuery;


/**
 * This widget allows the user to make selections based on an objects tags.
 */
class TagSelectWidget : public QTableWidget
{
Q_OBJECT
public:
	TagSelectWidget(QWidget* parent = nullptr);
	~TagSelectWidget() override;
	
	QWidget* makeButtons(QWidget* parent = nullptr);
	
	/**
	 * Builds a query based on the current state of the query table.
	 * 
	 * Returns an invalid query on error.
	 */
	ObjectQuery makeQuery() const;
	
protected:
	void showEvent(QShowEvent* event) override;
	
private:
	void addRow();
	void deleteRow();
	void moveRow(bool up);
	void moveRowDown();
	void moveRowUp();

	void addRowItems(int row);
	void onCellChanged(int row, int column);
	void updateRowButtons();
	
	QToolButton* delete_button;
	QToolButton* move_down_button;
	QToolButton* move_up_button;

	Q_DISABLE_COPY(TagSelectWidget)
};


}  // namespace OpenOrienteering

#endif
