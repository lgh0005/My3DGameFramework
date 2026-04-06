#pragma once
#include "Assets/RawAnimChannel.h"

namespace MGF3D
{
    struct RawAnimation
    {
        uint32 magic = 0x414E494D; // 'ANIM'
        uint32 version = 2;
        String name;
        float duration;
        float ticksPerSecond;

        float frameRate = 30.0f;   // 베이킹 FPS
        uint32 frameCount = 0;     // 총 프레임 수
        uint32 boneCount = 0;      // 뼈 개수

        Vector<glm::mat4> bakedMatrices;
        Vector<RawAnimChannel> channels;
    };

}