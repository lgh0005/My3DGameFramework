#include "GraphicsPch.h"
#include "ScreenMesh.h"
#include "Vertices/ScreenVertex.h"
#include "Layout/GLVertexLayout.h"
#include "Buffers/GLVertexBuffer.h"
#include "Buffers/GLIndexBuffer.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	ScreenMesh::ScreenMesh() = default;
	ScreenMesh::~ScreenMesh() = default;

	/*=====================//
	//   ScreenMesh Type   //
	//=====================*/
	int16 ScreenMesh::s_typeIndex = -1;
	const MGFType* ScreenMesh::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	ScreenMeshPtr ScreenMesh::Create()
	{
		auto mesh = ScreenMeshPtr(new ScreenMesh());
		mesh->Init();
		mesh->SetState(EResourceState::Loaded);
		return mesh;
	}

	void ScreenMesh::Init()
	{
		m_primitiveType = GL_TRIANGLES;

		// 1. 고정된 정점 및 인덱스 데이터 생성
		m_vertices =
		{
			// Pos(x,y,z)         // UV(u,v)
			ScreenVertex { {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f} }, // 0: Top-Left
			ScreenVertex { {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} }, // 1: Bottom-Left
			ScreenVertex { { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f} }, // 2: Top-Right
			ScreenVertex { { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} }  // 3: Bottom-Right
		};

		// 2. 인덱스 데이터
		m_indices = { 0, 1, 2, 2, 1, 3 };
		m_indexCount = m_indices.size();
	}

	bool ScreenMesh::OnSyncCreate()
	{
		// 3. 리소스 생성
		m_vertexLayout = GLVertexLayout::Create();
		m_vertexBuffer = GLVertexBuffer::Create(m_vertices.data(), m_vertices.size() * sizeof(ScreenVertex));
		m_indexBuffer = GLIndexBuffer::Create(m_indices.data(), m_indices.size() * sizeof(uint32));

		// 4. DSA 바인딩 및 레이아웃과 포맷 설정
		const uint32 bindingIndex = 0;
		m_vertexLayout->BindVertexBuffer(bindingIndex, m_vertexBuffer, 0, sizeof(ScreenVertex));
		m_vertexLayout->BindIndexBuffer(m_indexBuffer);
		m_vertexLayout->SetAttribFormat(0, 3, GL_FLOAT, false, offsetof(ScreenVertex, position), bindingIndex);
		m_vertexLayout->SetAttribFormat(2, 2, GL_FLOAT, false, offsetof(ScreenVertex, texCoord), bindingIndex);

		// 5. 속성 활성화
		m_vertexLayout->EnableAttrib(0);
		m_vertexLayout->EnableAttrib(2);

		m_state = EResourceState::Ready;
		return true;
	}

	void ScreenMesh::Draw(uint32 count) const
	{
		Bind();
		Mesh::Draw(count);
		Unbind();
	}
}
