#include "EnginePch.h"
#include "ScreenMesh.h"
#include "Resources/Material.h"
#include "Graphics/Buffers/Buffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Layouts/Vertex.h"
#include "Graphics/Layouts/VertexLayout.h"
#include "Resources/Programs/Program.h"

DECLARE_DEFAULTS_IMPL(ScreenMesh)

ScreenMeshUPtr ScreenMesh::Create()
{
	auto mesh = ScreenMeshUPtr(new ScreenMesh());
	mesh->Init();
	return std::move(mesh);
}

void ScreenMesh::Init()
{
	m_primitiveType = GL_TRIANGLES;

	// 고정된 정점 및 인덱스 데이터 생성
	std::vector<ScreenVertex> vertices = 
	{
					   // Pos(x,y,z)         // UV(u,v)
		ScreenVertex { {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f} }, // 0: Top-Left
		ScreenVertex { {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} }, // 1: Bottom-Left
		ScreenVertex { { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f} }, // 2: Top-Right
		ScreenVertex { { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} }  // 3: Bottom-Right
	};

	// 인덱스 설정
	std::vector<uint32> indices = { 0, 1, 2, 2, 1, 3 };
	m_indexCount = indices.size();
	
	// 버퍼 생성
	m_vertexLayout = VertexLayout::Create();
	m_vertexBuffer = VertexBuffer::Create(vertices.data(), sizeof(ScreenVertex), vertices.size());
	m_indexBuffer = IndexBuffer::Create(indices);

	// 정점 속성 설정
	m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(ScreenVertex), offsetof(ScreenVertex, position));
	m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(ScreenVertex), offsetof(ScreenVertex, texCoord));
}

void ScreenMesh::Draw() const
{
	m_vertexLayout->Bind();
	glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}
