#pragma once

namespace MGF3D
{
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

	template<typename T>
	void RenderBatch<T>::AddInstance(const T& data)
	{
		m_instances.push_back(data);
	}

	template<typename T>
	void RenderBatch<T>::Clear()
	{
		m_instances.clear();
	}
}