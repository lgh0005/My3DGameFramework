#include "GraphicsPch.h"
#include "Mesh.h"

namespace MGF3D
{
	Mesh::Mesh(const IResourceDescriptor& desc) : Super(desc) { }
	Mesh::~Mesh() = default;

	/*==================================//
	//         Type System              //
	//==================================*/
	UniquePtr<MGFType> Mesh::s_type = nullptr;

	void Mesh::InitializeType()
	{
		s_type = MakeUnique<MGFType>();
		s_type->name = MGFName("Mesh");
		s_type->id = StringHash("Mesh");

		// 부모  타입 상속
		const MGFType* parentType = Super::s_type.Get();
		if (parentType)
		{
			s_type->parent = parentType;
			s_type->depth = parentType->depth + 1;

			for (uint32 i = 0; i <= parentType->depth; ++i)
				s_type->chain[i] = parentType->chain[i];
		}
		s_type->chain[s_type->depth] = s_type->id;
	}

	const MGFType* Mesh::GetType() const
	{
		return s_type.Get();
	}

	void Mesh::AddSubMesh(SubMesh&& subMesh)
	{
		m_subMeshes.PushBack(std::move(subMesh));
	}

	const SVector<SubMesh>& Mesh::GetSubMeshes() const
	{
		return m_subMeshes;
	}

	usize Mesh::GetSubMeshCount() const
	{
		return m_subMeshes.Count();
	}
}