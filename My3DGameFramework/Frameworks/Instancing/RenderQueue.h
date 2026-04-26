#pragma once
#include "Instancing/RenderBatch.h"
#include "Graphics/Buffers/GLShaderStorageBuffer.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Mesh)
	MGF_CLASS_PTR(Material)

	template<typename T>
	class RenderQueue
	{
	public:
		RenderQueue(uint32 bindingSlot, usize initialCapacity = 1000);
		~RenderQueue();

	public:
		void Submit(Mesh* mesh, Material* material, const T& data);
		void Execute();
		void Clear();

	private:
		uint32 m_bindingSlot;
		GLShaderStorageBufferUPtr m_instanceBuffer;
		HashMap<Mesh*, HashMap<Material*, RenderBatch<T>>> m_batches;
	};
}

#include "Instancing/RenderQueue.inl"