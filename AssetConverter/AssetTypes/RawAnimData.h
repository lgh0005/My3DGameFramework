#pragma once

// INFO : 바이너리 파일 입출력 시 안정성을 위해
// 1바이트 정렬로 강제
#pragma pack(push, 1)

struct AnimHeader
{
    char     name[64];       // 애니메이션 이름
    float    duration;       // 전체 재생 시간 (Ticks)
    float    ticksPerSecond; // FPS
    uint32   channelCount;   // 트랙 개수
};

struct AnimChannelInfo
{
    uint32   positionCount;
    uint32   rotationCount;
    uint32   scaleCount;
};

struct KeyVector3
{
    float time;
    float x, y, z;
};

struct KeyQuaternion
{
    float time;
    float x, y, z, w;       // 명시적 순서 (x, y, z, w)
};

using AnimKeyPosition = KeyVector3;
using AnimKeyScale    = KeyVector3;
using AnimKeyRotation = KeyQuaternion;

#pragma pack(pop)