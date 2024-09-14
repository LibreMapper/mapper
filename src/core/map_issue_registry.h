/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_MAP_ISSUE_REGISTRY_H
#define LIBREMAPPER_MAP_ISSUE_REGISTRY_H

#include <unordered_map>
#include <vector>

#include <QString>


namespace LibreMapper {

class Map;
class MapIssuesTemplate;
class Object;

class MapIssueRegistry
{
public:
	enum class Severity
	{
		Critical,
		Warning,
		Info,
	};
	
	struct IssueEntry
	{
		Object const* object;
		Severity severity;
		QString explanation;
	};
	
	MapIssueRegistry() = delete;
	MapIssueRegistry(Map* map, MapIssuesTemplate* map_issues_view);

	void addObjectIssue(const Object* object, Severity severity, QString explanation);
	void deleteObjectIssue(const Object* object);
	std::vector<IssueEntry> getIssuesList();

private:
	Map* map = nullptr;
	MapIssuesTemplate* map_issues_view = nullptr;

	std::unordered_map<Object const*, IssueEntry> registered_issues;
};

} // namespace LibreMapper

#endif // LIBREMAPPER_MAP_ISSUE_REGISTRY_H
