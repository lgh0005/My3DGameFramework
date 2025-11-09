#pragma once

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

//struct VertexAnim
//{
//    glm::vec3 position;
//    glm::vec3 normal;
//    glm::vec2 texCoord;
//
//    glm::ivec4 boneIDs = glm::ivec4(-1, -1, -1, -1);
//    glm::vec4  boneWeights = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
//};