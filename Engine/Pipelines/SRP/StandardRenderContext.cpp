#include "EnginePch.h"
#include "StandardRenderContext.h"
#include "Graphics/Texture.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/CubeTexture.h"

void StandardRenderContext::SetGBuffer(Framebuffer* gBuffer)
{
	m_gBuffer = gBuffer;
	if (m_gBuffer)
	{
		m_gPosition = m_gBuffer->GetColorAttachment(0).get();
		m_gNormal = m_gBuffer->GetColorAttachment(1).get();
		m_gAlbedoSpec = m_gBuffer->GetColorAttachment(2).get();
		m_gEmission = m_gBuffer->GetColorAttachment(3).get();
	}
	else
	{
		m_gPosition = nullptr;
		m_gNormal = nullptr;
		m_gAlbedoSpec = nullptr;
		m_gEmission = nullptr;
	}
}

void StandardRenderContext::SetShadowMap(int32 index, Texture* texture)
{
	if (index >= 0 && index < MAX_SHADOW_CASTER)
	{
		m_shadowMaps[index] = texture;
	}
}

Texture* StandardRenderContext::GetShadowMap(int32 index) const
{
	if (index >= 0 && index < MAX_SHADOW_CASTER)
	{
		return m_shadowMaps[index];
	}
	return nullptr;
}

// TEMP : CullingPass에서 직접적으로 수행할 작업들.
// 현재는 일단 씬에 있는 것들을 그대로 복사.
#pragma region TEMP_SCENE_PROPERTY_COPY_METHODS
void StandardRenderContext::AddStaticMeshRenderer(MeshRenderer* renderer)
{
	m_culledStaticMeshRenderers.push_back(renderer);
}

void StandardRenderContext::AddSkinnedMeshRenderer(MeshRenderer* renderer)
{
	m_culledSkinnedMeshRenderers.push_back(renderer);
}

void StandardRenderContext::AddLight(Light* light)
{
	m_culledLights.push_back(light);
}
#pragma endregion

