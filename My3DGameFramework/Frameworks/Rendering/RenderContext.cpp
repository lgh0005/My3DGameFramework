#include "FrameworkPch.h"
#include "RenderContext.h"
#include "Buffers/GLUniformBuffer.h"

namespace MGF3D
{
	RenderContext::RenderContext() : m_staticQueue(1), m_skinnedQueue(1)
	{
        m_cameraUBO = GLUniformBuffer::Create(nullptr, sizeof(CameraData));

        m_dirLightSSBO = GLShaderStorageBuffer::Create(nullptr, 0);
        m_pointLightSSBO = GLShaderStorageBuffer::Create(nullptr, 0);
        m_spotLightSSBO = GLShaderStorageBuffer::Create(nullptr, 0);

        m_dirShadowSSBO = GLShaderStorageBuffer::Create(nullptr, 0);
        m_pointShadowSSBO = GLShaderStorageBuffer::Create(nullptr, 0);
        m_spotShadowSSBO = GLShaderStorageBuffer::Create(nullptr, 0);
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

    void RenderContext::UpdateDirectionalLights(const Vector<DirectionalLightData>& lights)
    {
        UpdateSSBO(m_dirLightSSBO, lights, 1);
    }

    void RenderContext::UpdatePointLights(const Vector<PointLightData>& lights)
    {
        UpdateSSBO(m_pointLightSSBO, lights, 2);
    }

    void RenderContext::UpdateSpotLights(const Vector<SpotLightData>& lights)
    {
        UpdateSSBO(m_spotLightSSBO, lights, 3);
    }

    void RenderContext::UpdateDirectionalShadows(const Vector<DirectionalShadowData>& shadows)
    {
        UpdateSSBO(m_dirShadowSSBO, shadows, 4);
    }

    void RenderContext::UpdatePointShadows(const Vector<PointShadowData>& shadows)
    {
        UpdateSSBO(m_pointShadowSSBO, shadows, 5);
    }

    void RenderContext::UpdateSpotShadows(const Vector<SpotShadowData>& shadows)
    {
        UpdateSSBO(m_spotShadowSSBO, shadows, 6);
    }
}