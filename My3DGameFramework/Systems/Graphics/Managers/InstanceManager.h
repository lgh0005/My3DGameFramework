#pragma once
#include "Instancing/InstanceProperty.h"

namespace MGF3D
{
	MGF_CLASS_PTR(RenderQueue)
	MGF_CLASS_PTR(Mesh)
	MGF_CLASS_PTR(Material)
	MGF_CLASS_PTR(Program)
	MGF_CLASS_PTR(AnimBuffer)

	MGF_CLASS_PTR(InstanceManager)
	class InstanceManager
	{
		MGF_DECLARE_SINGLE(InstanceManager)

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
