#pragma once

namespace MGF3D
{
    struct ScreenVertex
    {
        vec3 position;
        vec2 texCoord;
    };

    struct UIVertex
    {
        vec3 position;
        vec4 color;
        vec2 texCoord;
    };

    struct StaticVertex
    {
        vec3 position;
        vec3 normal;
        vec2 texCoord;
        vec3 tangent;
    };

    struct SkinnedVertex
    {
        vec3 position;
        vec3 normal;
        vec2 texCoord;
        vec3 tangent;
        int32     boneIDs[MAX_BONE_INFLUENCE];
        float     weights[MAX_BONE_INFLUENCE];
    };

    struct GizmoVertex
    {
        vec3 position;
        vec4 color;
    };
}
