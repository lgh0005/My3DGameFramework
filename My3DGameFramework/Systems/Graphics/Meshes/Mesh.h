#pragma once
#include "SubMesh.h"
#include "Resource/Resource.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Mesh)
	class Mesh : public Resource
	{
		using Super = Resource;

	public:
		Mesh(const IResourceDescriptor& desc);
		~Mesh();

	public:
		void AddSubMesh(SubMesh&& subMesh);
		const SVector<SubMesh>& GetSubMeshes() const;
		usize GetSubMeshCount() const;

	private:
		SVector<SubMesh> m_subMeshes;
	};
}