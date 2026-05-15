#include "FrameworkPch.h"
#include "RenderContext.h"
#include "Buffers/GLUniformBuffer.h"
#include "Meshes/ScreenMesh.h"
#include "Textures/GLTexture2D.h"
#include "Framebuffers/GLFramebuffer2D.h"
#include "Managers/ResourceManager.h"

namespace MGF3D
{
	RenderContext::RenderContext() : m_staticQueue(1), m_skinnedQueue(1)
	{
        m_globalUBO = GLUniformBuffer::Create(nullptr, sizeof(GlobalData));

        m_dirLightSSBO = GLShaderStorageBuffer::Create(nullptr, 0);
        m_pointLightSSBO = GLShaderStorageBuffer::Create(nullptr, 0);
        m_spotLightSSBO = GLShaderStorageBuffer::Create(nullptr, 0);

        m_gBufferTextures.reserve(static_cast<usize>(EGBufferSlot::Max));
        m_cachedTextures.resize(static_cast<usize>(ETextureCache::Max), nullptr);

        m_dirShadowSSBO = GLShaderStorageBuffer::Create(nullptr, 0);
        m_pointShadowSSBO = GLShaderStorageBuffer::Create(nullptr, 0);
        m_spotShadowSSBO = GLShaderStorageBuffer::Create(nullptr, 0);

        InitGeometryBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
        InitPostProcessBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
        m_screenMesh = MGF_RESOURCE.CreateImmediate<ScreenMesh>();
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
        m_dirLights = lights;
        UpdateSSBO(m_dirLightSSBO, lights, 3);
    }

    void RenderContext::UpdatePointLights(const Vector<PointLightData>& lights)
    {
        m_pointLights = lights;
        UpdateSSBO(m_pointLightSSBO, lights, 4);
    }

    void RenderContext::UpdateSpotLights(const Vector<SpotLightData>& lights)
    {
        m_spotLights = lights;
        UpdateSSBO(m_spotLightSSBO, lights, 5);
    }

    void RenderContext::UpdateDirectionalShadows(const Vector<DirectionalShadowData>& shadows)
    {
        m_dirShadows = shadows;
        UpdateSSBO(m_dirShadowSSBO, shadows, 6);
    }

    void RenderContext::UpdatePointShadows(const Vector<PointShadowData>& shadows)
    {
        m_pointShadows = shadows;
        UpdateSSBO(m_pointShadowSSBO, shadows, 7);
    }

    void RenderContext::UpdateSpotShadows(const Vector<SpotShadowData>& shadows)
    {
        m_spotShadows = shadows;
        UpdateSSBO(m_spotShadowSSBO, shadows, 8);
    }

    void RenderContext::InitGeometryBuffer(uint32 width, uint32 height)
    {
        // 1. 기존 텍스처를 비우고, 재할당을 막기 위해 메모리를 미리 예약
        m_gBufferTextures.clear();
        m_gBufferTextures.reserve(static_cast<usize>(EGBufferSlot::Max));

        // 2. G-Buffer 텍스쳐 생성 (다시 MGF3D 내부 규격인 Vulkan 포맷으로 원복!)
        // [0] Position (RGB) + AO (A) -> RGBA16F
        m_gBufferTextures.push_back(MGF_RESOURCE.CreateImmediate<GLTexture2D>(width, height, GL_RGBA16F, 1));

        // [1] Normal (RGB) + Roughness (A) -> RGBA16F
        m_gBufferTextures.push_back(MGF_RESOURCE.CreateImmediate<GLTexture2D>(width, height, GL_RGBA16F, 1));

        // [2] Albedo (RGB) + Metallic (A) -> RGBA8 Unorm
        m_gBufferTextures.push_back(MGF_RESOURCE.CreateImmediate<GLTexture2D>(width, height, GL_RGBA8, 1));

        // [3] Emission (RGB) + 잉여 (A) -> RGBA16F
        m_gBufferTextures.push_back(MGF_RESOURCE.CreateImmediate<GLTexture2D>(width, height, GL_RGBA16F, 1));

        // [4] Velocity (RG) -> RG16F
        m_gBufferTextures.push_back(MGF_RESOURCE.CreateImmediate<GLTexture2D>(width, height, GL_RG16F, 1));

        // 3. 깊이/스텐실 버퍼 텍스처 생성 -> Depth24 Stencil8
        auto depthAttachment = MGF_RESOURCE.CreateImmediate<GLTexture2D>(width, height, GL_DEPTH24_STENCIL8, 1);

        // 4. 무사히 핸들을 발급받은 텍스처들로 프레임버퍼 생성
        m_geometryBuffer = GLFramebuffer2D::Create(m_gBufferTextures, depthAttachment);
    }

    GLTexture2D* RenderContext::GetGeometryBufferTexture(EGBufferSlot slot) const
    {
        uint8 index = static_cast<uint8>(slot);
        if (index < m_gBufferTextures.size()) return m_gBufferTextures[index].get();
        return nullptr;
    }

    void RenderContext::InitPostProcessBuffer(uint32 width, uint32 height)
    {
        // 1. 조명 결과를 담을 HDR 텍스처 생성
        m_sceneColorTexture = MGF_RESOURCE.CreateImmediate<GLTexture2D>(width, height, GL_RGBA16F, 1);

        // 2. 텍스처를 포함하는 프레임버퍼 생성
        m_sceneBuffer = GLFramebuffer2D::Create({ m_sceneColorTexture }, m_geometryBuffer->GetDepthAttachment());
    }

    void RenderContext::SetCachedTexture(ETextureCache slot, const GLTexture2DPtr& texture)
    {
        uint8 index = static_cast<uint8>(slot);
        if (index < m_cachedTextures.size()) m_cachedTextures[index] = texture;
    }

    GLTexture2D* RenderContext::GetCachedTexture(ETextureCache slot) const
    {
        uint8 index = static_cast<uint8>(slot);
        if (index < m_cachedTextures.size()) return m_cachedTextures[index].get();
        return nullptr;
    }
}