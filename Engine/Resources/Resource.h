#pragma once
#include "Misc/Enums.h"

CLASS_PTR(Resource)
class Resource
{
public:
	virtual ~Resource();
	virtual ResourceType GetResourceType() const = 0;
	bool MatchesType(ResourceType type) const;

	const std::string& GetPath() const { return m_path; }
	void SetPath(const std::string& path) { m_path = path; }

	const std::string& GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

protected:
	Resource();
	std::string m_path;
	std::string m_name;
};