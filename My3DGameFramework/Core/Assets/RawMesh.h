#pragma once
#include "Assets/RawStaticVertex.h"
#include "Assets/RawSkinnedVertex.h"

namespace MGF3D
{
    struct RawMesh
    {
        String name;
        uint32 materialIndex;
        bool isSkinned = false;
        Vector<AssetFmt::RawStaticVertex>  staticVertices;
        Vector<AssetFmt::RawSkinnedVertex> skinnedVertices;
        Vector<uint32> indices;
        vec3 aabbMin = glm::vec3(MAX_FLOAT);
        vec3 aabbMax = glm::vec3(LOWEST_FLOAT);
    };
}