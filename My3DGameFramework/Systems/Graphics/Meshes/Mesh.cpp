#include "GraphicsPch.h"
#include "Mesh.h"
#include "Layout/GLVertexLayout.h"
#include "Buffers/GLVertexBuffer.h"
#include "Buffers/GLIndexBuffer.h"

namespace MGF3D
{
	Mesh::Mesh() = default;
	Mesh::~Mesh() = default;

	void Mesh::Draw(uint32 count) const
	{
		if (m_indexCount == 0) return;

		// 인스턴싱 드로우 콜
		glDrawElementsInstanced
		(
			m_primitiveType,
			static_cast<GLsizei>(m_indexCount),
			GL_UNSIGNED_INT,
			nullptr,
			static_cast<GLsizei>(count)
		);
	}

	void Mesh::Bind() const
	{
		if (m_vertexLayout) m_vertexLayout->Bind();
		if (m_indexBuffer) m_indexBuffer->Bind(m_vertexLayout->GetHandle());
	}

	void Mesh::Unbind() const
	{
		if (m_vertexLayout) m_vertexLayout->Unbind();
		if (m_indexBuffer) m_indexBuffer->Unbind(m_vertexLayout->GetHandle());
	}
}

