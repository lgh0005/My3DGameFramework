#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Mesh)
	MGF_CLASS_PTR(Material)
	MGF_CLASS_PTR(GLShaderStorageBuffer)

	template<typename T>
	class RenderBatch
	{
	public:
		void Draw
		(
			uint32 bindingSlot,
			GLShaderStorageBuffer* instanceBuffer
		);

	public:
		void SetMesh(Mesh* mesh);
		void SetMaterial(Material* material);
		void AddInstance(const T& data);
		Vector<T>& GetInstances() { return m_instances; }
		void Clear();

	private:
		Mesh*	  m_mesh     { nullptr };
		Material* m_material { nullptr };
		Vector<T> m_instances;
	};
}

#include "Instancing/RenderBatch.inl"