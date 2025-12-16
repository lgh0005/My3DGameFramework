#pragma once
#include "Resource.h"

CLASS_PTR(Skeleton)
class Skeleton : public Resource
{
public:
    static const ResourceType s_ResourceType = ResourceType::Skeleton;
    virtual ResourceType GetResourceType() const override { return ResourceType::Skeleton; }
};