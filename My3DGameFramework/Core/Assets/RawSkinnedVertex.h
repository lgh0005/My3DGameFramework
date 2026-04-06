#pragma once

namespace MGF3D
{
    struct RawSkinnedVertex
    {
        RawSkinnedVertex();

        vec3 position;
        vec3 normal;
        vec2 texCoord;
        vec3 tangent;
        int32     boneIDs[MAX_BONE_INFLUENCE];
        float     weights[MAX_BONE_INFLUENCE];
    };
}