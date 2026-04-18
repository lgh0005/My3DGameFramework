#include "GraphicsPch.h"
#include "GeometryUtils.h"
#include "Vertices/StaticVertex.h"
#include "Meshes/StaticMesh.h"

namespace MGF3D
{
    StaticMeshPtr GeometryUtils::CreateBox()
    {
        Vector<StaticVertex> vertices =
        {
            StaticVertex { vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f,  0.0f, -1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(0.5f, -0.5f, -0.5f), vec3(0.0f,  0.0f, -1.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(0.5f,  0.5f, -0.5f), vec3(0.0f,  0.0f, -1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(-0.5f,  0.5f, -0.5f), vec3(0.0f,  0.0f, -1.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },

            StaticVertex { vec3(-0.5f, -0.5f,  0.5f), vec3(0.0f,  0.0f,  1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(0.5f, -0.5f,  0.5f), vec3(0.0f,  0.0f,  1.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(0.5f,  0.5f,  0.5f), vec3(0.0f,  0.0f,  1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(-0.5f,  0.5f,  0.5f), vec3(0.0f,  0.0f,  1.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },

            StaticVertex { vec3(-0.5f,  0.5f,  0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) },
            StaticVertex { vec3(-0.5f,  0.5f, -0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f) },
            StaticVertex { vec3(-0.5f, -0.5f, -0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f) },
            StaticVertex { vec3(-0.5f, -0.5f,  0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) },

            StaticVertex { vec3(0.5f,  0.5f,  0.5f), vec3(1.0f,  0.0f,  0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) },
            StaticVertex { vec3(0.5f,  0.5f, -0.5f), vec3(1.0f,  0.0f,  0.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f) },
            StaticVertex { vec3(0.5f, -0.5f, -0.5f), vec3(1.0f,  0.0f,  0.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f) },
            StaticVertex { vec3(0.5f, -0.5f,  0.5f), vec3(1.0f,  0.0f,  0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) },

            StaticVertex { vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f,  0.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f,  0.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(0.5f, -0.5f,  0.5f), vec3(0.0f, -1.0f,  0.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(-0.5f, -0.5f,  0.5f), vec3(0.0f, -1.0f,  0.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },

            StaticVertex { vec3(-0.5f,  0.5f, -0.5f), vec3(0.0f,  1.0f,  0.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(0.5f,  0.5f, -0.5f), vec3(0.0f,  1.0f,  0.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(0.5f,  0.5f,  0.5f), vec3(0.0f,  1.0f,  0.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
            StaticVertex { vec3(-0.5f,  0.5f,  0.5f), vec3(0.0f,  1.0f,  0.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
        };

        Vector<uint32> indices =
        {
           0,  2,  1,  2,  0,  3,
           4,  5,  6,  6,  7,  4,
           8,  9, 10, 10, 11,  8,
          12, 14, 13, 14, 12, 15,
          16, 17, 18, 18, 19, 16,
          20, 22, 21, 22, 20, 23,
        };

        vec3 minBound(MAX_FLOAT);
        vec3 maxBound(MIN_FLOAT);
        for (const auto& v : vertices)
        {
            minBound = Math::Min(minBound, v.position);
            maxBound = Math::Max(maxBound, v.position);
        }

        auto mesh = StaticMesh::Create(std::move(vertices), std::move(indices), GL_TRIANGLES);
        if (!mesh) return nullptr;

        RenderBounds bounds;
        bounds.SetFromMinMax(minBound, maxBound);
        mesh->SetLocalBounds(bounds);

        return mesh;
    }

    StaticMeshPtr GeometryUtils::CreatePlane()
    {
        Vector<StaticVertex> vertices =
        {
                StaticVertex { vec3(-0.5f, -0.5f, 0.0f), vec3(0.0f,  0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
                StaticVertex { vec3(0.5f, -0.5f, 0.0f), vec3(0.0f,  0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
                StaticVertex { vec3(0.5f,  0.5f, 0.0f), vec3(0.0f,  0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },
                StaticVertex { vec3(-0.5f,  0.5f, 0.0f), vec3(0.0f,  0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) },
        };

        Vector<uint32> indices = { 0, 1, 2, 2, 3, 0 };

        vec3 minBound(MAX_FLOAT);
        vec3 maxBound(MIN_FLOAT);
        for (const auto& v : vertices)
        {
            minBound = Math::Min(minBound, v.position);
            maxBound = Math::Max(maxBound, v.position);
        }

        auto mesh = StaticMesh::Create(std::move(vertices), std::move(indices), GL_TRIANGLES);
        if (!mesh) return nullptr;

        RenderBounds bounds;
        bounds.SetFromMinMax(minBound, maxBound);
        mesh->SetLocalBounds(bounds);

        return mesh;
    }
   
    StaticMeshPtr GeometryUtils::CreateSphere
    (
        uint32 latiSegmentCount,
        uint32 longiSegmentCount
    )
    {
        Vector<StaticVertex> vertices;
        Vector<uint32> indices;

        uint32 circleVertCount = longiSegmentCount + 1;
        vertices.resize((latiSegmentCount + 1) * circleVertCount);
        for (uint32 i = 0; i <= latiSegmentCount; i++)
        {
            float v = (float)i / (float)latiSegmentCount;
            float phi = (v - 0.5f) * PI;
            auto cosPhi = Math::Cos(phi);
            auto sinPhi = Math::Sin(phi);
            for (uint32 j = 0; j <= longiSegmentCount; j++)
            {
                float u = (float)j / (float)longiSegmentCount;
                float theta = u * TWO_PI;
                auto cosTheta = Math::Cos(theta);
                auto sinTheta = Math::Sin(theta);
                auto point = vec3(cosPhi * cosTheta, sinPhi, -cosPhi * sinTheta);
                vertices[i * circleVertCount + j] = StaticVertex{ point * 0.5f, point, vec2(u, v), vec3(0.0f) };
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
        vec3 minBound(MAX_FLOAT);
        vec3 maxBound(MIN_FLOAT);
        for (const auto& v : vertices)
        {
            minBound = Math::Min(minBound, v.position);
            maxBound = Math::Max(maxBound, v.position);
        }

        auto mesh = StaticMesh::Create(std::move(vertices), std::move(indices), GL_TRIANGLES);
        if (!mesh) return nullptr;

        RenderBounds bounds;
        bounds.SetFromMinMax(minBound, maxBound);
        mesh->SetLocalBounds(bounds);
        return mesh;
    }
}