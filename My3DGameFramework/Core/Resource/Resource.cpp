#include "CorePch.h"
#include "Resource.h"

namespace MGF3D
{
    Resource::Resource(StringHash id)
        : typeID(id), m_state(ResourceState::Unloaded) { }
    Resource::~Resource() = default;
}