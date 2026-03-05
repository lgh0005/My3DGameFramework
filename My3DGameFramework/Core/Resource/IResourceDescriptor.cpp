#include "CorePch.h"
#include "IResourceDescriptor.h"

namespace MGF3D
{
	IResourceDescriptor::IResourceDescriptor(StringHash id) : typeID(id) { }
	IResourceDescriptor::~IResourceDescriptor() = default;
}