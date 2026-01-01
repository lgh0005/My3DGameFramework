#include "EnginePch.h"
#include "UniversalOutlinePass.h"

#include "Graphics/RenderContext.h"
#include "Scene/GameObject.h"
#include "Graphics/Program.h"
#include "Framebuffers/OutlineFramebuffer.h"
#include "Framebuffers/PostProcessFramebuffer.h"
#include "Resources/Mesh.h"
#include "Resources/ScreenMesh.h"
#include "Resources/Texture.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/MeshOutline.h"
#include "Components/Animator.h"
#include "Components/Transform.h"

#include "Pipelines/URP/UniversalRenderContext.h"
#include "Pipelines/URP/RenderPasses/UniversalPostProcessPass.h"

UniversalOutlinePass::UniversalOutlinePass() = default;
UniversalOutlinePass::~UniversalOutlinePass() = default;

UniversalOutlinePassUPtr UniversalOutlinePass::Create
(
	int32 width, int32 height,
	const glm::vec3& color,
	float thickness)
{
	auto pass = UniversalOutlinePassUPtr(new UniversalOutlinePass());
	if (!pass->Init(width, height, color, thickness)) return nullptr;
	return std::move(pass);
}

bool UniversalOutlinePass::Init
(
	int32 width, int32 height,
	const glm::vec3& color,
	float thickness
)
{
	m_color = color;
	m_thickness = thickness;

	// 1. Static Mask Program
	m_maskStaticProgram = Program::Create
	(
		"./Resources/Shaders/Common/Common_Outline_Mask_Static.vert",
		"./Resources/Shaders/Common/Common_Outline_Mask.frag"
	);

	// 2. Skinned Mask Program
	m_maskSkinnedProgram = Program::Create
	(
		"./Resources/Shaders/Common/Common_Outline_Mask_Skinned.vert",
		"./Resources/Shaders/Common/Common_Outline_Mask.frag"
	);

	// 3. Post Process Program
	m_postProgram = Program::Create
	(
		"./Resources/Shaders/Common/Common_Outline_Post.vert",
		"./Resources/Shaders/Common/Common_Outline_Post.frag"
	);
	if (!m_maskStaticProgram || !m_maskSkinnedProgram || !m_postProgram)
		return false;

	m_maskFBO = OutlineFramebuffer::Create(width, height);
	m_screenMesh = ScreenMesh::Create();

	return (m_maskFBO && m_screenMesh);
}

void UniversalOutlinePass::Render(RenderContext* context)
{
	// 0. 자신의 렌더 패스에 활용되고 있는 RenderContext로 캐스팅
	auto stdCtx = (UniversalRenderContext*)context;

	// 1. 그릴 대상이 없으면 빠른 리턴
	const auto& outlines = context->GetMeshOutlines();
	if (outlines.empty()) return;

	// [Pass 1] 마스크 텍스처 생성 (Mask Generation)
	m_maskFBO->Bind();

	// 배경은 검은색(0,0,0)으로 클리어
	glViewport(0, 0, m_maskFBO->GetWidth(), m_maskFBO->GetHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// [핵심] 깊이 테스트 끄기 (Always On Top / X-Ray)
	// 벽 뒤에 있든 말든 무조건 마스크를 그립니다.
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE); // 구멍 없이 꽉 채우기 위해 컬링 끔

	// 마스킹 렌더링 (단색으로 그리기)
	MaskMeshes(outlines);

	glDepthFunc(GL_LESS);

	// 다시 메인 화면(디폴트 FBO)으로 복귀
	auto* target = stdCtx->GetTargetFramebuffer();
	if (target)
	{
		target->Bind();
		glViewport(0, 0, target->GetWidth(), target->GetHeight());
	}
	else
	{
		// TODO : 뷰포트 바꾸는 로직 수정 필요
		Framebuffer::BindToDefault();
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	// [Pass 2] 합성 (Composition)
	// 화면 전체에 덮어씌울 것이므로 깊이 테스트 끔
	glDisable(GL_DEPTH_TEST);

	// 투명한 배경(discard)과 합성하기 위해 블렌딩 켜기
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_postProgram->Use();

	// 0번 슬롯에 마스크 텍스처 바인딩
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_maskFBO->GetTexture()->Get());
	m_postProgram->SetUniform("stencilTexture", 0);

	// 해상도 및 두께 전달
	m_postProgram->SetUniform("canvasSize", glm::vec2(m_maskFBO->GetWidth(), m_maskFBO->GetHeight()));
	m_postProgram->SetUniform("outlineSize", m_thickness);
	m_postProgram->SetUniform("outlineColor", glm::vec4(m_color, 1.0f));

	// ScreenMesh로 화면 꽉 차게 그리기
	m_screenMesh->Draw();

	// 상태 복구
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

/*==============================//
//   outlining helper methods   //
//==============================*/
void UniversalOutlinePass::MaskMeshes(const std::vector<MeshOutline*>& outlines)
{
	MaskStaticMeshes(outlines);
	MaskSkinnedMeshes(outlines);
}

void UniversalOutlinePass::MaskStaticMeshes(const std::vector<MeshOutline*>& outlines)
{
	m_maskStaticProgram->Use();

	for (auto* outline : outlines)
	{
		if (!outline->IsEnabled()) continue;

		auto* owner = outline->GetOwner();
		if (!owner || !owner->IsActiveInHierarchy()) continue;

		// StaticMeshRenderer만 골라냄
		auto* renderer = owner->GetComponent<StaticMeshRenderer>();
		if (!renderer || !renderer->IsEnabled()) continue;

		// Static용 유니폼 설정 (Bone 필요 없음)
		m_maskStaticProgram->SetUniform("model", renderer->GetTransform().GetWorldMatrix());
		m_maskStaticProgram->SetUniform("outlineColor", outline->GetColor());

		renderer->Render(m_maskStaticProgram.get());
	}
}

void UniversalOutlinePass::MaskSkinnedMeshes(const std::vector<MeshOutline*>& outlines)
{
	m_maskSkinnedProgram->Use();
	for (auto* outline : outlines)
	{
		if (!outline->IsEnabled()) continue;

		auto* owner = outline->GetOwner();
		if (!owner || !owner->IsActiveInHierarchy()) continue;

		// SkinnedMeshRenderer만 골라냄
		auto* renderer = owner->GetComponent<SkinnedMeshRenderer>();
		if (!renderer || !renderer->IsEnabled()) continue;

		// Skinned용 유니폼 설정
		m_maskSkinnedProgram->SetUniform("model", renderer->GetTransform().GetWorldMatrix());
		m_maskSkinnedProgram->SetUniform("outlineColor", outline->GetColor());

		// Bone 행렬 전달
		Animator* animator = renderer->GetAnimator();
		if (animator && animator->IsEnabled() && animator->GetOwner()->IsActiveInHierarchy())
			m_maskSkinnedProgram->SetUniform("finalBoneMatrices", animator->GetFinalBoneMatrices());
		else m_maskSkinnedProgram->SetUniform("finalBoneMatrices", GetIdentityBones());
		renderer->Render(m_maskSkinnedProgram.get());
	}
}