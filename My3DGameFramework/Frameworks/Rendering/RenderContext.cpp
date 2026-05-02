#include "FrameworkPch.h"
#include "RenderContext.h"
#include "Buffers/GLUniformBuffer.h"

namespace MGF3D
{
	RenderContext::RenderContext() : m_staticQueue(1), m_skinnedQueue(1)
	{
        m_globalUBO = GLUniformBuffer::Create(nullptr, sizeof(GlobalData));

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

    void RenderContext::UpdateGlobals(const GlobalData& globalData)
    {
        if (!m_globalUBO) return;
		m_globalUBO->UpdateData(&globalData, 0, sizeof(GlobalData));
		m_globalUBO->Bind(0);
    }

    void RenderContext::UpdateDirectionalLights(const Vector<DirectionalLightData>& lights)
    {
        UpdateSSBO(m_dirLightSSBO, lights, 3);
    }

    void RenderContext::UpdatePointLights(const Vector<PointLightData>& lights)
    {
        UpdateSSBO(m_pointLightSSBO, lights, 4);
    }

    void RenderContext::UpdateSpotLights(const Vector<SpotLightData>& lights)
    {
        UpdateSSBO(m_spotLightSSBO, lights, 5);
    }

    void RenderContext::UpdateDirectionalShadows(const Vector<DirectionalShadowData>& shadows)
    {
        UpdateSSBO(m_dirShadowSSBO, shadows, 6);
    }

    void RenderContext::UpdatePointShadows(const Vector<PointShadowData>& shadows)
    {
        UpdateSSBO(m_pointShadowSSBO, shadows, 7);
    }

    void RenderContext::UpdateSpotShadows(const Vector<SpotShadowData>& shadows)
    {
        UpdateSSBO(m_spotShadowSSBO, shadows, 8);
    }
}