#pragma once

struct alignas(16) InstanceProperty
{
    glm::mat4 worldMatrix   { 1.0f };
    glm::vec4 color         { 1.0f };
    uint32  boneOffset      { 0 };      // 4 bytes (Skinned용)
    uint32  typeFlag        { 0 };      // 4 bytes (0: Static, 1: Skinned, 2: Outline 등)
    uint32  pad[2]          { 0 };      // 8 bytes
};