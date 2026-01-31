#pragma once

struct alignas(16) InstanceProperty
{
    glm::mat4 worldMatrix{ 1.0f };
};

struct alignas(16) StaticInstanceProperty
{
    InstanceProperty common;
};

struct alignas(16) SkinnedInstanceProperty
{
    InstanceProperty common;             // 80 bytes
    uint32    boneOffset  { 0 };         // 4 bytes
    uint32    pad1[3]     { 0 };         // 12 bytes (16바이트 정렬: 4 + 12 = 16)
};