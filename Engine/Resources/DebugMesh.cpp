#include "EnginePch.h"
#include "DebugMesh.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Buffer.h"

DECLARE_DEFAULTS_IMPL(DebugMesh)

DebugMeshUPtr DebugMesh::Create(uint32 primitiveType)
{
	auto mesh = DebugMeshUPtr(new DebugMesh());
	mesh->Init(primitiveType);
	return std::move(mesh);
}

void DebugMesh::Init(uint32 primitiveType)
{
	m_primitiveType = primitiveType;

	// 1. VAO 생성
	m_vertexLayout = VertexLayout::Create();
	m_vertexLayout->Bind();

	// 2. 초기 빈 버퍼 생성
	m_vertexBuffer = Buffer::CreateWithData
	(
		GL_ARRAY_BUFFER,      // bufferType
		GL_DYNAMIC_DRAW,      // usage
		nullptr,              // data (빈 공간만 확보)
		sizeof(GizmoVertex),  // stride
		1000                  // count
	);
	m_bufferCapacity = 1000 * sizeof(GizmoVertex);

	// 3. 속성 설정 (GizmoVertex 전용)
	m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(GizmoVertex), offsetof(GizmoVertex, position));
	m_vertexLayout->SetAttrib(1, 4, GL_FLOAT, false, sizeof(GizmoVertex), offsetof(GizmoVertex, color));
}

void DebugMesh::Draw() const
{
	if (m_vertexCount == 0 || !m_vertexLayout) return;
	m_vertexLayout->Bind();
	glDrawArrays(m_primitiveType, 0, m_vertexCount);
}

void DebugMesh::UpdateGeometry(const std::vector<GizmoVertex>& vertices)
{
	m_vertexCount = (uint32)vertices.size();
	if (m_vertexCount == 0) return;

	usize dataSize = vertices.size() * sizeof(GizmoVertex);

	// 안전장치: VAO 바인딩
	if (m_vertexLayout) m_vertexLayout->Bind();

	// 1. 용량 부족 시 재할당
	if (!m_vertexBuffer || m_bufferCapacity < dataSize)
	{
		m_bufferCapacity = dataSize * 2;
		usize count = m_bufferCapacity / sizeof(GizmoVertex);
		m_vertexBuffer = Buffer::CreateWithData
		(
			GL_ARRAY_BUFFER,
			GL_DYNAMIC_DRAW,
			nullptr,
			sizeof(GizmoVertex),
			count
		);

		m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(GizmoVertex), offsetof(GizmoVertex, position));
		m_vertexLayout->SetAttrib(1, 4, GL_FLOAT, false, sizeof(GizmoVertex), offsetof(GizmoVertex, color));

		m_vertexBuffer->SetData(vertices.data(), dataSize);
	}
	else
	{
		m_vertexBuffer->SetData(vertices.data(), dataSize);
	}
}