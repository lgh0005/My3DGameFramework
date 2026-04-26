#pragma once

namespace MGF3D
{
	template<typename T>
	void RenderBatch<T>::Clear()
	{
		m_instances.clear();
	}

	template<typename T>
	void RenderBatch<T>::SetMesh(Mesh* mesh)
	{
		m_mesh = mesh;
	}

	template<typename T>
	void RenderBatch<T>::SetMaterial(Material* material)
	{
		m_material = material;
	}
}