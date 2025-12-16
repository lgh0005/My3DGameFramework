#pragma once
#include "Resources/Resource.h"

CLASS_PTR(AnimationChannel)
class AnimationChannel : public Resource
{
public:
	static const ResourceType s_ResourceType = ResourceType::AnimationChannel;
	virtual ResourceType GetResourceType() const override { return ResourceType::AnimationChannel; }

};