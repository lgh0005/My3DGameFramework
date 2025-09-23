#include "pch.h"
#include "IResource.h"

IResource::IResource(const string& name, Resources::ResourceType type) : _name(name), _type(type)
{
}
