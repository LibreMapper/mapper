/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013, 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2014, 2015, 2017-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_CRS_TEMPLATE_IMPLEMENTATION_H
#define LIBREMAPPER_CRS_TEMPLATE_IMPLEMENTATION_H

#include "crs_template.h"

#include <utility>
#include <vector>

#include <QLineEdit>
#include <QString>
#include <QVariant>

class QWidget;

namespace OpenOrienteering {

class LatLon;


/**
 * This namespace collects CRS template implementations
 */
namespace CRSTemplates {

/**
 * Creates and returns a list of known CRS Templates.
 */
CRSTemplateRegistry::TemplateList defaultList();



/**
 * CRSTemplate parameter specifying a text parameter.
 */
class TextParameter : public CRSTemplateParameter
{
public:
	TextParameter(const QString& id, const QString& name);
	QWidget* createEditor(WidgetObserver& observer) const override;
	QString value(const QWidget* edit_widget) const override;
	void setValue(QWidget* edit_widget, const QString& value) override;
	
protected:
	/// The type of editor widget returned from createEditor.
	using Editor = QLineEdit;
	
};



/**
 * CRSTemplate parameter giving a full specification.
 */
class FullSpecParameter : public TextParameter
{
public:
	FullSpecParameter(const QString& id, const QString& name);
	QWidget* createEditor(WidgetObserver& observer) const override;
	void setValue(QWidget* edit_widget, const QString& value) override;
};



/**
 * CRSTemplate parameter specifying a UTM zone.
 */
class UTMZoneParameter : public CRSTemplateParameter
{
public:
	UTMZoneParameter(const QString& id, const QString& name);
	QWidget* createEditor(WidgetObserver& observer) const override;
	std::vector<QString> specValues(const QString& edit_value) const override;
	QString value(const QWidget* edit_widget) const override;
	void setValue(QWidget* edit_widget, const QString& value) override;
	
	/**
	 * Determines the UTM zone from the given latitude and longitude.
	 * 
	 * Returns a null value on error.
	 */
	static QVariant calculateUTMZone(const LatLon& lat_lon);
};



/**
 * CRSTemplate integer parameter, with values from an integer range.
 */
class IntRangeParameter : public CRSTemplateParameter
{
public:
	using OutputList = std::vector< std::pair<int, int> >;
	
	IntRangeParameter(const QString& id, const QString& name, int min_value, int max_value);
	IntRangeParameter(const QString& id, const QString& name, int min_value, int max_value, OutputList&& outputs);
	QWidget* createEditor(WidgetObserver& observer) const override;
	std::vector<QString> specValues(const QString& edit_value) const override;
	QString value(const QWidget* edit_widget) const override;
	void setValue(QWidget* edit_widget, const QString& value) override;
	
private:
	const int min_value;
	const int max_value;
	const OutputList outputs;
};


}  // namespace CRSTemplates

}  // namespace OpenOrienteering

#endif
