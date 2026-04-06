#include "EnginePch.h"
#include "Mesh.h"
#include "Resources/Material.h"
#include "Graphics/Buffers/Buffer.h"
#include "Graphics/Layouts/VertexLayout.h"
#include "Resources/Programs/Program.h"

DECLARE_DEFAULTS_IMPL(Mesh)

void Mesh::Bind() const
{
	if (m_vertexLayout) m_vertexLayout->Bind();
	if (m_indexBuffer) m_indexBuffer->Bind();
}

void Mesh::Unbind() const
{
	if (m_vertexLayout) m_vertexLayout->Unbind();
	if (m_indexBuffer) m_indexBuffer->Unbind();
}

void Mesh::RenderInstanced(uint32 count) const
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
