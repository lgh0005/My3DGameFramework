#include "FrameworkPch.h"
#include "RenderContext.h"
#include "Buffers/GLUniformBuffer.h"

namespace MGF3D
{
	RenderContext::RenderContext() : m_staticQueue(1), m_skinnedQueue(1)
	{
		m_cameraUBO = GLUniformBuffer::Create(nullptr, sizeof(CameraData));
		m_lightUBO = GLUniformBuffer::Create(nullptr, sizeof(LightData));
	}
	RenderContext::~RenderContext() = default;

    void RenderContext::ClearQueues() 
    {
        m_staticQueue.Clear();
        m_skinnedQueue.Clear();
    }

    void RenderContext::UpdateCameras(const CameraData& camera)
    {
        if (!m_cameraUBO) return;
		m_cameraUBO->UpdateData(&camera, 0, sizeof(CameraData));
		m_cameraUBO->Bind(0);
    }

    void RenderContext::UpdateLights(const LightData& lights)
    {
        if (!m_lightUBO) return;
        m_lightUBO->UpdateData(&lights, 0, sizeof(LightData));
        m_lightUBO->Bind(3);
    }

}