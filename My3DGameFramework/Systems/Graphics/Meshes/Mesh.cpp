#include "GraphicsPch.h"
#include "Mesh.h"
#include "Managers/TypeManager.h"
#include "Layout/GLVertexLayout.h"

namespace MGF3D
{
	Mesh::Mesh() = default;
	Mesh::~Mesh() = default;

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

	void Mesh::Unbind()
	{
		m_vertexLayout->Unbind();
	}
}

