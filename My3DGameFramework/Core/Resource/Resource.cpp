#include "CorePch.h"
#include "Resource.h"

namespace MGF3D
{
    Resource::Resource(const IResourceDescriptor& desc)
        : m_typeID(desc.typeID)
        , m_name(desc.name)
        , m_path(desc.path)
    { }

    Resource::~Resource() = default;
}