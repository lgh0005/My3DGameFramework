#include "EnginePch.h"
#include "WorldSpaceEffects.h"
#include "Graphics/RenderContext.h"
#include "Resources/Program.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Graphics/Framebuffers/PostProcessFrameBuffer.h"
#include "Components/Camera.h"

DECLARE_DEFAULTS_IMPL(WorldSpaceEffects)

WorldSpaceEffectsUPtr WorldSpaceEffects::Create(int32 priority, int32 width, int32 height)
{
	auto effects = WorldSpaceEffectsUPtr(new WorldSpaceEffects());
	if (!effects->Init(priority, width, height)) return nullptr;
	return std::move(effects);
}

bool WorldSpaceEffects::Init(int32 priority, int32 width, int32 height)
{
	m_priority = priority;
	m_width = width;
	m_height = height;
	m_worldSpaceEffectProgram = RESOURCE.GetResource<Program>("world_space_effects");

	return m_worldSpaceEffectProgram != nullptr;
}

bool WorldSpaceEffects::Render(RenderContext* context, Framebuffer* src, Framebuffer* dst, ScreenMesh* scrn)
{
	if (!context || !src || !dst || !scrn) return false;
	auto camera = context->GetCamera();
	if (!camera) return false;

	dst->Bind();
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST); // 포스트 프로세싱은 깊이 테스트가 필요 없음
	glDisable(GL_CULL_FACE);  // 사각형이 뒤집혀 보일 가능성 차단

	m_worldSpaceEffectProgram->Use();

	// [Input 0] Scene Color (HDR)
	glActiveTexture(GL_TEXTURE0);
	src->GetColorAttachment(0)->Bind();
	m_worldSpaceEffectProgram->SetUniform("uSceneTex", 0);

	// [Input 1] Depth Texture (G-Buffer에서 가져옴)
	// TODO : 깊이도 GBuffer에 기록을 해야함
	Texture* depthTex = context->GetTexture(RenderSlot::GVelocity);
	if (depthTex)
	{
		glActiveTexture(GL_TEXTURE1);
		depthTex->Bind();
		m_worldSpaceEffectProgram->SetUniform("uDepthTex", 1);
	}

	// [Uniforms] 파라미터 전송
	m_worldSpaceEffectProgram->SetUniform("uPixelSize", m_pixelSize);
	m_worldSpaceEffectProgram->SetUniform("uPostLevels", m_postLevels);
	m_worldSpaceEffectProgram->SetUniform("uDilationRadius", m_dilationRadius);
	m_worldSpaceEffectProgram->SetUniform("uFogColor", m_fogColor);
	m_worldSpaceEffectProgram->SetUniform("uFogDensity", m_fogDensity);

	scrn->Draw();

	glEnable(GL_DEPTH_TEST);

	return true;
}
