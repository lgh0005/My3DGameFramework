#include "CorePch.h"
#include "Resource.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
    Resource::Resource(const IResourceDescriptor& desc)
        : Super(desc.type, desc.name) { }
    Resource::~Resource() = default;
}