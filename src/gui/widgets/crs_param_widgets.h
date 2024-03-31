/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2015, 2017-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_CRS_PARAM_WIDGETS_H
#define LIBREMAPPER_CRS_PARAM_WIDGETS_H

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QWidget>

class QLineEdit;

namespace LibreMapper {

class CRSParameterWidgetObserver;


class UTMZoneEdit : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(UTMZoneEdit)
public:
	UTMZoneEdit(CRSParameterWidgetObserver& observer, QWidget* parent = nullptr);
	~UTMZoneEdit() override;
	
	QString text() const;
	void setText(const QString& text);
	bool calculateValue();
	
signals:
	void textChanged(const QString& text);
	
private:
	CRSParameterWidgetObserver& observer;
	QLineEdit* line_edit;
};


}  // namespace LibreMapper

#endif
