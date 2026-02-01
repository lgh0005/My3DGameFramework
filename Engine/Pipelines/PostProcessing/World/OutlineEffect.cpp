#include "EnginePch.h"
#include "OutlineEffect.h"
#include "Object/GameObject.h"
#include "Graphics/Rendering/RenderContext.h"
#include "Graphics/Framebuffers/OutlineFramebuffer.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Textures/Texture.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/MeshOutline.h"
#include "Components/Animator.h"
#include "Components/Transform.h"
#include "Instancing/RenderQueue.h"

DECLARE_DEFAULTS_IMPL(OutlineEffect)

OutlineEffectUPtr OutlineEffect::Create
(
	int32 priority, int32 width, int32 height,
	const glm::vec3& color, float thickness
)
{
	auto effect = OutlineEffectUPtr(new OutlineEffect());
	if (!effect->Init(priority, width, height, color, thickness)) return nullptr;
	return std::move(effect);
}

bool OutlineEffect::Init
(
	int32 priority, int32 width, int32 height,
	const glm::vec3& color, float thickness
)
{
	m_color = color;
	m_thickness = thickness;
	m_width = width;
	m_height = height;

	m_maskInstancedProgram = RESOURCE.GetResource<GraphicsProgram>("postprocess_outline_mask");
	m_postProgram = RESOURCE.GetResource<GraphicsProgram>("postprocess_outline_post");
	m_maskFBO = OutlineFramebuffer::Create(width, height);
	m_renderQueue = RenderQueue::Create(1024, 512 * MAX_BONES);
	if (!m_maskInstancedProgram || !m_postProgram || !m_maskFBO || !m_renderQueue)
		return false;

	return true;
}

bool OutlineEffect::Render
(
	RenderContext* context, 
	Framebuffer* srcFBO, Framebuffer* dstFBO, 
	ScreenMesh* screenMesh
)
{
	// 0. 데이터 확인
	if (!context || !srcFBO || !dstFBO) return false;
	const auto& outlines = context->GetMeshOutlines();
	if (outlines.empty())
	{
		Framebuffer::Blit(srcFBO, dstFBO, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		return true;
	}

	// [Pass 1] 마스크 텍스처 생성 (Mask Generation)
	m_maskFBO->Bind();
	glViewport(0, 0, m_maskFBO->GetWidth(), m_maskFBO->GetHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	CollectOutlines(outlines);

	// [Pass 2] 최종 합성
	dstFBO->Bind();
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT);

	m_postProgram->Use();

	// 0번 슬롯: 이전 패스 장면 (srcFBO)
	glActiveTexture(GL_TEXTURE0);
	srcFBO->GetColorAttachment(0)->Bind();
	m_postProgram->SetUniform("uSceneTexture", 0);

	// 1번 슬롯: 아웃라인 마스크 (m_maskFBO)
	glActiveTexture(GL_TEXTURE1);
	m_maskFBO->GetTexture()->Bind();
	m_postProgram->SetUniform("stencilTexture", 1);

	// 아웃라인 색상 및 두께 전달
	m_postProgram->SetUniform("outlineSize", m_thickness);
	m_postProgram->SetUniform("outlineColor", glm::vec4(m_color, 1.0f));
	m_postProgram->SetUniform("canvasSize", glm::vec2(m_width, m_height));

	// ScreenMesh로 화면 꽉 차게 그리기
	screenMesh->Draw();

	// 상태 복구
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	return true;
}

void OutlineEffect::CollectOutlines(const std::vector<MeshOutline*>& outlines)
{
	m_renderQueue->Clear();

	for (auto* outline : outlines)
	{
		if (!outline->IsEnabled() || !outline->GetOwner()->IsActive()) continue;

		MeshRenderer* renderer = outline->GetMeshRenderer();
		if (!renderer || !renderer->IsEnabled()) continue;

		// 통합 구조체 채우기
		InstanceProperty prop;
		prop.worldMatrix = renderer->GetTransform().GetWorldMatrix();
		prop.color = glm::vec4(outline->GetColor(), 1.0f);

		// 스킨드 메쉬인 경우 뼈 데이터 추출
		switch (renderer->GetComponentType())
		{
		case ComponentType::StaticMeshRenderer:
		{
			m_renderQueue->Add(renderer->GetMesh().get(), nullptr, prop);
			break;
		}

		case ComponentType::SkinnedMeshRenderer:
		{
			auto* skinned = static_cast<SkinnedMeshRenderer*>(renderer);
			Animator* animator = skinned->GetAnimator();
			const auto& bones = (animator && animator->IsEnabled())
				? animator->GetFinalBoneMatrices()
				: Utils::GetIdentityBones();

			m_renderQueue->Add(renderer->GetMesh().get(), nullptr, prop, bones);
			break;
		}
		}
	}

	if (m_maskInstancedProgram)
	{
		m_maskInstancedProgram->Use();
		m_renderQueue->Execute(m_maskInstancedProgram.get());
	}
}

void OutlineEffect::OnResize(int32 width, int32 height)
{
	// 1. 멤버 변수 및 FBO 리사이즈
	Super::OnResize(width, height);
	m_maskFBO->OnResize(m_width, m_height);

	// 2. 셰이더에게 변경된 해상도 알려주기
	if (m_postProgram)
	{
		m_postProgram->Use();
		m_postProgram->SetUniform("canvasSize", glm::vec2(width, height));
	}
}
