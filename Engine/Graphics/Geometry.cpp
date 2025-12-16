#include "EnginePch.h"
#include "Geometry.h"
#include "Graphics/Vertex.h"
#include "Resources/StaticMesh.h"

/*===================//
//   Plane methods   //
//===================*/
CullingPlane::CullingPlane(const glm::vec3& normal, float dist)
    : m_normal(normal), m_distance(dist) {}

CullingPlane CullingPlane::Create(const glm::vec4& eq)
{
    return CullingPlane(glm::vec3(eq.x, eq.y, eq.z), eq.w);
}

void CullingPlane::Normalize()
{
    float length = glm::length(m_normal);
    if (length < glm::epsilon<float>()) return;

    float invLength = 1.0f / length;
    m_normal *= invLength;
    m_distance *= invLength;
}

float CullingPlane::GetDistanceToPoint(const glm::vec3& point) const
{
    return glm::dot(m_normal, point) + m_distance;
}

// [DEBUG]
void CullingPlane::Flip()
{
    m_normal *= -1.0f;
    m_distance *= -1.0f;
}

/*==========================//
//   RenderBounds methods   //
//==========================*/
RenderBounds::RenderBounds() : m_center(0.0f), m_extents(0.0f) {}

RenderBounds RenderBounds::Create(const glm::vec3& center, 
                                  const glm::vec3& extents)
{
    RenderBounds bound;
    bound.m_center = center;
    bound.m_extents = extents;
    return bound;
}

RenderBounds RenderBounds::CreateFromMinMax(const glm::vec3& min, const glm::vec3& max)
{
    return RenderBounds::Create((max + min) * 0.5f, (max - min) * 0.5f);
}

RenderBounds RenderBounds::Transform(const glm::mat4& mat) const
{
    glm::vec3 newCenter = glm::vec3(mat * glm::vec4(m_center, 1.0f));
    glm::vec3 right = glm::vec3(mat[0]);
    glm::vec3 up = glm::vec3(mat[1]);
    glm::vec3 fwd = glm::vec3(mat[2]);

    glm::vec3 newExtents;
    newExtents.x = glm::abs(right.x) * m_extents.x + glm::abs(up.x) * m_extents.y + glm::abs(fwd.x) * m_extents.z;
    newExtents.y = glm::abs(right.y) * m_extents.x + glm::abs(up.y) * m_extents.y + glm::abs(fwd.y) * m_extents.z;
    newExtents.z = glm::abs(right.z) * m_extents.x + glm::abs(up.z) * m_extents.y + glm::abs(fwd.z) * m_extents.z;

    return RenderBounds::Create(newCenter, newExtents);
}

RenderBounds RenderBounds::Union(const RenderBounds& other) const
{
    // 1. 현재 bounds를 Min/Max 좌표로 변환
    glm::vec3 minA = m_center - m_extents;
    glm::vec3 maxA = m_center + m_extents;

    // 2. other bounds를 Min/Max 좌표로 변환
    glm::vec3 minB = other.m_center - other.m_extents;
    glm::vec3 maxB = other.m_center + other.m_extents;

    // 3. 두 Min/Max를 합쳐서 새로운 최종 Min/Max 계산 (Union)
    glm::vec3 newMin = Utils::Min(minA, minB);
    glm::vec3 newMax = Utils::Max(maxA, maxB);

    return RenderBounds::CreateFromMinMax(newMin, newMax);
}

/*===============================//
//   GeometryGenerator methods   //
//===============================*/
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
        minBound = Utils::Min(minBound, v.position);
        maxBound = Utils::Max(maxBound, v.position);
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
        minBound = Utils::Min(minBound, v.position);
        maxBound = Utils::Max(maxBound, v.position);
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
    for (uint32_t i = 0; i < latiSegmentCount; i++)
    {
        for (uint32_t j = 0; j < longiSegmentCount; j++)
        {
            uint32_t vertexOffset = i * circleVertCount + j;
            uint32_t indexOffset = (i * longiSegmentCount + j) * 6;
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
        minBound = Utils::Min(minBound, v.position);
        maxBound = Utils::Max(maxBound, v.position);
    }

    auto mesh = StaticMesh::Create(vertices, indices, GL_TRIANGLES);
    mesh->SetLocalBounds(RenderBounds::CreateFromMinMax(minBound, maxBound));
    if (!mesh) return nullptr;
    return std::move(mesh);
}