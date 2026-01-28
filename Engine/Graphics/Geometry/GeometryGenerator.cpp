#include "EnginePch.h"
#include "GeometryGenerator.h"
#include "Graphics/Layouts/Vertex.h"
#include "Resources/Meshes/StaticMesh.h"

StaticMeshUPtr GeometryGenerator::CreateBox()
{
    std::vector<StaticVertex> vertices =
    {
        StaticVertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },

        StaticVertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },

        StaticVertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
        StaticVertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
        StaticVertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
        StaticVertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },

        StaticVertex { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },

        StaticVertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },

        StaticVertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        StaticVertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
    };

    std::vector<uint32> indices =
    {
       0,  2,  1,  2,  0,  3,
       4,  5,  6,  6,  7,  4,
       8,  9, 10, 10, 11,  8,
      12, 14, 13, 14, 12, 15,
      16, 17, 18, 18, 19, 16,
      20, 22, 21, 22, 20, 23,
    };

    glm::vec3 minBound(FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX);
    for (const auto& v : vertices)
    {
        minBound = Utils::GlmVec3Min(minBound, v.position);
        maxBound = Utils::GlmVec3Max(maxBound, v.position);
    }

    auto mesh = StaticMesh::Create(vertices, indices, GL_TRIANGLES);
    mesh->SetLocalBounds(RenderBounds::CreateFromMinMax(minBound, maxBound));
    if (!mesh) return nullptr;

    return std::move(mesh);
}

StaticMeshUPtr GeometryGenerator::CreatePlane()
{
    std::vector<StaticVertex> vertices =
    {
            StaticVertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
    };

    std::vector<uint32> indices = { 0, 1, 2, 2, 3, 0 };

    glm::vec3 minBound(FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX);
    for (const auto& v : vertices)
    {
        minBound = Utils::GlmVec3Min(minBound, v.position);
        maxBound = Utils::GlmVec3Max(maxBound, v.position);
    }

    auto mesh = StaticMesh::Create(vertices, indices, GL_TRIANGLES);
    mesh->SetLocalBounds(RenderBounds::CreateFromMinMax(minBound, maxBound));
    if (!mesh) return nullptr;

    return std::move(mesh);
}

StaticMeshUPtr GeometryGenerator::CreateSphere
(
    uint32 latiSegmentCount,
    uint32 longiSegmentCount
)
{
    std::vector<StaticVertex> vertices;
    std::vector<uint32> indices;

    uint32 circleVertCount = longiSegmentCount + 1;
    vertices.resize((latiSegmentCount + 1) * circleVertCount);
    for (uint32 i = 0; i <= latiSegmentCount; i++)
    {
        float v = (float)i / (float)latiSegmentCount;
        float phi = (v - 0.5f) * glm::pi<float>();
        auto cosPhi = cosf(phi);
        auto sinPhi = sinf(phi);
        for (uint32 j = 0; j <= longiSegmentCount; j++)
        {
            float u = (float)j / (float)longiSegmentCount;
            float theta = u * glm::pi<float>() * 2.0f;
            auto cosTheta = cosf(theta);
            auto sinTheta = sinf(theta);
            auto point = glm::vec3
            (
                cosPhi * cosTheta, sinPhi, -cosPhi * sinTheta
            );

            vertices[i * circleVertCount + j] = StaticVertex
            {
                point * 0.5f, point, glm::vec2(u, v), glm::vec3(0.0f)
            };
        }
    }

    indices.resize(latiSegmentCount * longiSegmentCount * 6);
    for (uint32 i = 0; i < latiSegmentCount; i++)
    {
        for (uint32 j = 0; j < longiSegmentCount; j++)
        {
            uint32 vertexOffset = i * circleVertCount + j;
            uint32 indexOffset = (i * longiSegmentCount + j) * 6;
            indices[indexOffset] = vertexOffset;
            indices[indexOffset + 1] = vertexOffset + 1;
            indices[indexOffset + 2] = vertexOffset + 1 + circleVertCount;
            indices[indexOffset + 3] = vertexOffset;
            indices[indexOffset + 4] = vertexOffset + 1 + circleVertCount;
            indices[indexOffset + 5] = vertexOffset + circleVertCount;
        }
    }

    // 3. 바운딩 박스(AABB) 계산
    glm::vec3 minBound(FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX);
    for (const auto& v : vertices)
    {
        minBound = Utils::GlmVec3Min(minBound, v.position);
        maxBound = Utils::GlmVec3Max(maxBound, v.position);
    }

    auto mesh = StaticMesh::Create(vertices, indices, GL_TRIANGLES);
    mesh->SetLocalBounds(RenderBounds::CreateFromMinMax(minBound, maxBound));
    if (!mesh) return nullptr;
    return std::move(mesh);
}