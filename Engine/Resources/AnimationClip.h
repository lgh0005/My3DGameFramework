#pragma once
#include "Resources/Resource.h"

CLASS_PTR(AnimationClip)
class AnimationClip : public Resource
{
public:
	static const ResourceType s_ResourceType = ResourceType::AnimationClip;
	virtual ResourceType GetResourceType() const override { return ResourceType::AnimationClip; }

};