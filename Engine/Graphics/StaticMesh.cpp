#include "EnginePch.h"
#include "StaticMesh.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Material.h"
#include "Graphics/Program.h"

StaticMesh::~StaticMesh() = default;

StaticMeshUPtr StaticMesh::Create(const std::vector<StaticVertex>& vertices,
	                              const std::vector<uint32>& indices,
	                              uint32 primitiveType)
{
	auto mesh = StaticMeshUPtr(new StaticMesh());
	mesh->Init(vertices, indices, primitiveType);
	return std::move(mesh);
}

void StaticMesh::Init(const std::vector<StaticVertex>& vertices,
                      const std::vector<uint32>& indices,
                      uint32 primitiveType)
{
    m_primitiveType = primitiveType;
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData
    (
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(StaticVertex), vertices.size()
    );
    m_indexBuffer = Buffer::CreateWithData
    (
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32), indices.size()
    );
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(StaticVertex), 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(StaticVertex), offsetof(StaticVertex, normal));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(StaticVertex), offsetof(StaticVertex, texCoord));
}

void StaticMesh::Draw(const Program* program) const
{
    m_vertexLayout->Bind();
    if (m_material) m_material->SetToProgram(program);
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}

/*==============//
//  mesh utils  //
//==============*/
#pragma region MESH_UTILS

StaticMeshUPtr StaticMesh::CreateBox()
{
    std::vector<StaticVertex> vertices =
    {
        StaticVertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
        StaticVertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },

        StaticVertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
        StaticVertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },

        StaticVertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        StaticVertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        StaticVertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        StaticVertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        StaticVertex { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        StaticVertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        StaticVertex { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        StaticVertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        StaticVertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        StaticVertex { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        StaticVertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
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

    return Create(vertices, indices, GL_TRIANGLES);
}

StaticMeshUPtr StaticMesh::CreatePlane()
{
    std::vector<StaticVertex> vertices =
    {
            StaticVertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
            StaticVertex { glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
            StaticVertex { glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
            StaticVertex { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
    };

    std::vector<uint32> indices = { 0, 1, 2, 2, 3, 0 };

    return Create(vertices, indices, GL_TRIANGLES);
}

#pragma endregion