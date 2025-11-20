#pragma once
#define MAX_BONE_INFLUENCE 4

struct StaticVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
};

struct SkinnedVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    int32     boneIDs[MAX_BONE_INFLUENCE];
    float     weights[MAX_BONE_INFLUENCE];
};