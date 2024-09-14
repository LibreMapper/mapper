/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */


#include "map_issue_registry.h"

#include <utility>

#include <QString>

#include "core/map.h"
#include "core/objects/object.h"
#include "templates/map_issues_template.h"

namespace LibreMapper {

MapIssueRegistry::MapIssueRegistry(Map* map, MapIssuesTemplate* map_issues_view)
    : map(map)
    , map_issues_view(map_issues_view)

{
	// nothing
}

void MapIssueRegistry::addObjectIssue(Object const* object, Severity severity, QString explanation)
{
	registered_issues.emplace(std::make_pair(object, IssueEntry {object, severity, explanation}));
	map->makeMapIssuesVisible();
	map->setTemplateAreaDirty(map_issues_view, object->getExtent(), 0);	
}

void MapIssueRegistry::deleteObjectIssue(Object const* object)
{
	registered_issues.erase(object);
	map->setTemplateAreaDirty(map_issues_view, object->getExtent(), 0);
}

std::vector<MapIssueRegistry::IssueEntry> MapIssueRegistry::getIssuesList()
{
	std::vector<IssueEntry> ret;
	
	for (auto const& issue : registered_issues)
		ret.push_back(issue.second);  
	
	return ret;
}

} // namespace LibreMapper
