#include "CorePch.h"
#include "ScreenMesh.h"
#include "Graphics/Vertices/Vertex.h"
#include "Graphics/Layout/GLVertexLayout.h"
#include "Graphics/Buffers/GLVertexBuffer.h"
#include "Graphics/Buffers/GLIndexBuffer.h"

namespace MGF3D
{
	ScreenMesh::ScreenMesh() = default;
	ScreenMesh::~ScreenMesh() = default;

	ScreenMeshPtr ScreenMesh::Create()
	{
		auto mesh = ScreenMeshPtr(new ScreenMesh());
		mesh->Init();
		return mesh;
	}

	void ScreenMesh::Init()
	{
		m_primitiveType = GL_TRIANGLES;

		// 1. 고정된 정점 및 인덱스 데이터 생성
		Vector<ScreenVertex> vertices =
		{
			// Pos(x,y,z)         // UV(u,v)
			ScreenVertex { {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f} }, // 0: Top-Left
			ScreenVertex { {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} }, // 1: Bottom-Left
			ScreenVertex { { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f} }, // 2: Top-Right
			ScreenVertex { { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} }  // 3: Bottom-Right
		};

		// 2. 인덱스 데이터
		Vector<uint32> indices = { 0, 1, 2, 2, 1, 3 };
		m_indexCount = indices.size();

		// 3. 리소스 생성
		m_vertexLayout = GLVertexLayout::Create();
		m_vertexBuffer = GLVertexBuffer::Create(vertices.data(), vertices.size() * sizeof(ScreenVertex));
		m_indexBuffer = GLIndexBuffer::Create(indices.data(), indices.size() * sizeof(uint32));

		// 4. DSA 바인딩 및 레이아웃과 포맷 설정
		const uint32 bindingIndex = 0;
		m_vertexLayout->BindVertexBuffer(bindingIndex, m_vertexBuffer, 0, sizeof(ScreenVertex));
		m_vertexLayout->BindIndexBuffer(m_indexBuffer);
		m_vertexLayout->SetAttribFormat(0, 3, GL_FLOAT, false, offsetof(ScreenVertex, position), bindingIndex);
		m_vertexLayout->SetAttribFormat(2, 2, GL_FLOAT, false, offsetof(ScreenVertex, texCoord), bindingIndex);

		// 속성 활성화
		m_vertexLayout->EnableAttrib(0);
		m_vertexLayout->EnableAttrib(2);
	}

	void ScreenMesh::Draw(uint32 count) const
	{
		// 1. VAO 바인딩 (이제 이 안에 VBO, EBO 바인딩 정보가 다 들어있음)
		Bind();

		// 2. 부모의 공통 드로우 콜 호출
		Mesh::Draw(count);

		// 3. 바인딩 해제
		Unbind();
	}
}
