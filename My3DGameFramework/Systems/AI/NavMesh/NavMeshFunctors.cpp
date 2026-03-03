#include "EnginePch.h"
#include "NavMeshFunctors.h"

namespace MGF3D
{
    void DetourNavMeshDeleter::operator()(dtNavMesh* ptr) const
    {
        if (ptr) dtFreeNavMesh(ptr);
    }

    void DetourNavMeshQueryDeleter::operator()(dtNavMeshQuery* ptr) const
    {
        if (ptr) dtFreeNavMeshQuery(ptr);
    }
}