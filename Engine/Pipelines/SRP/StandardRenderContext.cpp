#include "EnginePch.h"
#include "StandardRenderContext.h"
#include "Framebuffers/PostProcessFramebuffer.h"
#include "Framebuffers/GBufferFramebuffer.h"
#include "Resources/Texture.h"

DECLARE_DEFAULTS_IMPL(StandardRenderContext)

void StandardRenderContext::SetGBuffer(GBufferFramebuffer* gBuffer)
{
	m_gBuffer = gBuffer;
	if (m_gBuffer)
	{
		m_gPosition = m_gBuffer->GetColorAttachment(0).get();
		m_gNormal = m_gBuffer->GetColorAttachment(1).get();
		m_gAlbedoSpec = m_gBuffer->GetColorAttachment(2).get();
		m_gEmission = m_gBuffer->GetColorAttachment(3).get();
		m_gVelocity = m_gBuffer->GetColorAttachment(4).get();
	}
	else
	{
		m_gPosition = nullptr;
		m_gNormal = nullptr;
		m_gAlbedoSpec = nullptr;
		m_gEmission = nullptr;
		m_gVelocity = nullptr;
	}
}

void StandardRenderContext::BindTargetFramebuffer()
{
	// 1. 포스트 프로세스 프레임 버퍼를 사전 바인딩
	// 이를 통해서 외부에서 포워드 렌더 패스를 만들 때 그릴 도화지 자체를 몰라도 됨.
	if (m_targetFramebuffer)
	{
		m_targetFramebuffer->Bind();
		glViewport(0, 0, m_targetFramebuffer->GetWidth(), m_targetFramebuffer->GetHeight());
	}
	else
	{
		Framebuffer::BindToDefault();
	}

	// 상태 오염을 방지하기 위한 기본 상태 세팅
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}
