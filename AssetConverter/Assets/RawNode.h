#pragma once

namespace MGF3D
{
    struct RawNode
    {
        String name;
        int32 parentIndex = -1;     // -1이면 루트
        mat4 localTransform;        // 초기 변환 (T-Pose)
        Vector<uint32> meshIndices;
        Vector<int32> children;
    };
}