#pragma once
#include "Resource.h"

CLASS_PTR(Pose)
class Pose : public Resource
{
public:
    static const ResourceType s_ResourceType = ResourceType::Pose;
    virtual ResourceType GetResourceType() const override { return ResourceType::Pose; }
};