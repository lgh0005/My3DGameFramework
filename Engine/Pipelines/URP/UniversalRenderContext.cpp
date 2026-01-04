#include "EnginePch.h"
#include "UniversalRenderContext.h"
#include "Framebuffers/PostProcessFramebuffer.h"
#include "Framebuffers/GBufferFramebuffer.h"
#include "Resources/Texture.h"

DECLARE_DEFAULTS_IMPL(UniversalRenderContext)

void UniversalRenderContext::SetGBuffer(GBufferFramebuffer* gBuffer)
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