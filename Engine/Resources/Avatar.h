#pragma once
#include "Resources/Resource.h"

CLASS_PTR(Avatar)
class Avatar : public Resource
{
public:
	static const ResourceType s_ResourceType = ResourceType::Avatar;
	virtual ResourceType GetResourceType() const override { return ResourceType::Avatar; }

};