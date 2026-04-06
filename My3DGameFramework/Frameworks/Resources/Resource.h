#pragma once
#include "Misc/Enums.h"
#include "Resources/ResourceDesc.h"

CLASS_PTR(Resource)
class Resource
{
public:
	virtual ~Resource();
	virtual ResourceType GetResourceType() const = 0;
	virtual ResourceDesc& GetDesc() = 0;
	virtual const ResourceDesc& GetDesc() const = 0;
	bool MatchesType(ResourceType type) const;
	const std::string& GetName() const;

protected:
	Resource();
};