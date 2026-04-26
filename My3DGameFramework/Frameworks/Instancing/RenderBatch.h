#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Mesh)
	MGF_CLASS_PTR(Material)

	template<typename T>
	class RenderBatch
	{
	public:
		void Clear();
		void SetMesh(Mesh* mesh);
		void SetMaterial(Material* material);

	private:
		Mesh*	  m_mesh     { nullptr };
		Material* m_material { nullptr };
		Vector<T> m_instances;
	};
}

#include "Instancing/RenderBatch.inl"