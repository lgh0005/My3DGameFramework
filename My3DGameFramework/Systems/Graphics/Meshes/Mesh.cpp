#include "GraphicsPch.h"
#include "Mesh.h"
#include "Layout/GLVertexLayout.h"
#include "Buffers/GLVertexBuffer.h"
#include "Buffers/GLIndexBuffer.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Mesh::Mesh() = default;
	Mesh::~Mesh() = default;

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

	/*==========================//
	//   GLTextureHandle Type   //
	//==========================*/
	int16 Mesh::s_typeIndex = -1;
	const MGFType* Mesh::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}

