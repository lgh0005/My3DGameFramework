#pragma once
#include <Recast.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>

struct DetourNavMeshDeleter
{
    void operator()(dtNavMesh* ptr) const;
};

struct DetourNavMeshQueryDeleter
{
    void operator()(dtNavMeshQuery* ptr) const;
};

using NavMeshUPtr = std::unique_ptr<dtNavMesh, DetourNavMeshDeleter>;
using NavMeshQueryUPtr = std::unique_ptr<dtNavMeshQuery, DetourNavMeshQueryDeleter>;
using RecastContextUPtr = std::unique_ptr<rcContext>;