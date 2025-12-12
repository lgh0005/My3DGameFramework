#include "EnginePch.h"
#include "ScreenMesh.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Material.h"
#include "Graphics/Program.h"

ScreenMesh::~ScreenMesh() = default;

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

	std::vector<uint32> indices = 
	{
		0, 1, 2, // 첫 번째 삼각형
		2, 1, 3  // 두 번째 삼각형
	};
	m_indexCount = indices.size();
	
	// 버퍼 생성
	m_vertexLayout = VertexLayout::Create();
	m_vertexBuffer = Buffer::CreateWithData
	(
		GL_ARRAY_BUFFER, GL_STATIC_DRAW,
		vertices.data(), sizeof(ScreenVertex), vertices.size()
	);
	m_indexBuffer = Buffer::CreateWithData
	(
		GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
		indices.data(), sizeof(uint32), indices.size()
	);

	// 정점 속성 설정
	m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(ScreenVertex), offsetof(ScreenVertex, position));
	m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(ScreenVertex), offsetof(ScreenVertex, texCoord));
}

void ScreenMesh::Draw(const Program* program) const
{
	m_vertexLayout->Bind();
	glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}
