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
        Vector<RawStaticVertex>  staticVertices;
        Vector<RawSkinnedVertex> skinnedVertices;
        Vector<uint32> indices;
        vec3 aabbMin = vec3(MAX_FLOAT);
        vec3 aabbMax = vec3(LOWEST_FLOAT);
    };
}