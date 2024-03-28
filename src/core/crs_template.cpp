/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013, 2014 Thomas Schöps (OpenOrienteering)
 * Copyright 2014, 2015 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "crs_template.h"

#include <iterator>
#include <utility>

#include <QtGlobal>
#include <QLatin1Char>


namespace OpenOrienteering {

// From crs_template_implementation.h/.cpp
namespace CRSTemplates
{
	CRSTemplateRegistry::TemplateList defaultList();
}



// ### CRSParameterWidgetObserver ###

CRSParameterWidgetObserver::~CRSParameterWidgetObserver() = default;



// ### CRSTemplateParameter ###

CRSTemplateParameter::CRSTemplateParameter(const QString& id, const QString& name)
 : param_id(id)
 , param_name(name)
{
	// nothing
}

CRSTemplateParameter::~CRSTemplateParameter()
{
	// nothing, not inlined
}

std::vector<QString> CRSTemplateParameter::specValues(const QString& edit_value) const
{
	return { edit_value };
}



// ### CRSTemplate ###

CRSTemplate::CRSTemplate(
        const QString& id,
        const QString& name,
        const QString& coordinates_name,
        const QString& spec_template,
        ParameterList&& parameters)
 : template_id(id)
 , template_name(name)
 , coordinates_name(coordinates_name)
 , spec_template(spec_template)
 , params(std::move(parameters))
{
	// nothing
}

CRSTemplate::~CRSTemplate()
{
	for (auto&& param : params)
		delete param;
}

QString CRSTemplate::coordinatesName(const std::vector<QString>& values) const
{
	Q_ASSERT(params.size() == values.size()
	         || values.empty());
	
	auto name = coordinates_name;
	
	auto value = begin(values);
	auto last_value = end(values);
	for (auto key = begin(params), last = end(params);
	     key != last && value != last_value;
	     ++key, ++value)
	{
		name.replace(QLatin1Char('@') + (*key)->id() + QLatin1Char('@'), *value);
	}
	
	return name;
}



// ### CRSTemplateRegistry ###

CRSTemplateRegistry::CRSTemplateRegistry()
{
	static auto shared_list = CRSTemplates::defaultList();
	this->templates = &shared_list;
}

const CRSTemplate* CRSTemplateRegistry::find(const QString& id) const
{
	const CRSTemplate* ret = nullptr;
	for (auto&& temp : *templates)
	{
		if (temp->id() == id) {
			ret = temp.get();
			break;
		}
			
	}
	return ret;
}

void CRSTemplateRegistry::add(std::unique_ptr<const CRSTemplate> temp)
{
	templates->push_back(std::move(temp));
}


}  // namespace OpenOrienteering
