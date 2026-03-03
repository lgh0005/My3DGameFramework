#pragma once
#include "Instancing/InstanceProperty.h"

namespace MGF3D
{
	CLASS_PTR(RenderQueue)
	CLASS_PTR(Mesh)
	CLASS_PTR(Material)
	CLASS_PTR(Program)
	CLASS_PTR(AnimBuffer)

	CLASS_PTR(InstanceManager)
	class InstanceManager
	{
		DECLARE_SINGLE(InstanceManager)

	public:
		bool Init();

		void BuildGlobalAnimBuffer();
		void BindGlobalAnimBuffer(uint32 slot = 10);

		void Submit(Mesh* mesh, Material* material, const InstanceProperty& prop);
		void Render(Program* program);
		void ClearQueue();

	private:
		InstanceManager();
		~InstanceManager();

		AnimBufferUPtr m_globalAnimBuffer;
		RenderQueueUPtr m_globalInstanceQueue;
	};
}
