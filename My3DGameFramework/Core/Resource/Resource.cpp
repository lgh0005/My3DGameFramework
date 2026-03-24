#include "CorePch.h"
#include "Resource.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
    Resource::Resource(const IResourceDescriptor& desc)
        : m_type(desc.type), m_name(desc.name) { }
    Resource::~Resource() = default;
}