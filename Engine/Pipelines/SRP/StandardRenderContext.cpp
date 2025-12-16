#include "EnginePch.h"
#include "StandardRenderContext.h"
#include "Graphics/FrameBuffer.h"
#include "Resources/Texture.h"
#include "Resources/CubeTexture.h"

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
