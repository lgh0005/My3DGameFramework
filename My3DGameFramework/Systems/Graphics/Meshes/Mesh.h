#pragma once
#include "SubMesh.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Mesh)
	class Mesh
	{
	public:
		Mesh();
		~Mesh();

	public:
		void AddSubMesh(SubMesh&& subMesh);
		const SVector<SubMesh>& GetSubMeshes() const;
		usize GetSubMeshCount() const;

	private:
		SVector<SubMesh> m_subMeshes;
	};
}