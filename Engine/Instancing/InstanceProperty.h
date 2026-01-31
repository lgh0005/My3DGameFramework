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

struct alignas(16) OutlineStaticInstanceProperty
{
    InstanceProperty common;
    glm::vec4 outlineColor{ 1.0f, 1.0f, 0.0f, 1.0f };
};

struct alignas(16) OutlineSkinnedInstanceProperty
{
    InstanceProperty common;
    glm::vec4 outlineColor{ 1.0f, 1.0f, 0.0f, 1.0f };
    uint32    boneOffset{ 0 };
    uint32    pad[3]{ 0, 0, 0 };
};