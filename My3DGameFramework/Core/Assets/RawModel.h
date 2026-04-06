#pragma once
#include "Assets/RawMaterial.h"
#include "Assets/RawMesh.h"
#include "Assets/RawBoneInfo.h"
#include "Assets/RawNode.h"

namespace MGF3D
{
    struct RawModel
    {
        uint32 magicNumber = 0x4D594D44; // 'MYMD'
        uint32 version = 2;

        Vector<RawMaterial> materials;
        Vector<RawMesh> meshes;
        Vector<RawBoneInfo> boneOffsetInfos;

        bool hasSkeleton = false;

        Vector<RawNode> nodes;
        vec3 globalAABBMin = vec3(MAX_FLOAT);
        vec3 globalAABBMax = vec3(LOWEST_FLOAT);
    };
}