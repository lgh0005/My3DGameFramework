#pragma once
#include "Instancing/RenderQueue.h"
#include "Instancing/Properties/StaticInstanceData.h"
#include "Instancing/Properties/SkinnedInstanceData.h"
#include "Uniforms/CameraUniform.h"
#include "Uniforms/LightUniform.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLUniformBuffer)

	MGF_CLASS_PTR(RenderContext)
	class RenderContext
	{
	public:
		RenderContext();
		~RenderContext();

	public:
		auto& GetStaticQueue() { return m_staticQueue; }
		auto& GetSkinnedQueue() { return m_skinnedQueue; }
		void ClearQueues();

	public:
		void UpdateCameras(const CameraData& camera);
		void UpdateLights(const LightData& lights);

	private:
		RenderQueue<StaticInstanceData>  m_staticQueue;
		RenderQueue<SkinnedInstanceData> m_skinnedQueue;

		GLUniformBufferUPtr m_cameraUBO;
		GLUniformBufferUPtr m_lightUBO;
	};
}