#include "EnginePch.h"
#include "OutlineEffect.h"
#include "Object/GameObject.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Framebuffers/OutlineFramebuffer.h"
#include "Resources/Program.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Textures/Texture.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/MeshOutline.h"
#include "Components/Animator.h"
#include "Components/Transform.h"

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

	m_maskStaticProgram = RESOURCE.GetResource<Program>("common_outline_static");
	m_maskSkinnedProgram = RESOURCE.GetResource<Program>("common_outline_skinned");
	m_postProgram = RESOURCE.GetResource<Program>("common_outline_postprocess");
	m_maskFBO = OutlineFramebuffer::Create(width, height);
	if (!m_maskStaticProgram || !m_maskSkinnedProgram || !m_postProgram || !m_maskFBO)
		return false;

	m_postProgram->Use();
	m_postProgram->SetUniform("stencilTexture", 0);
	m_postProgram->SetUniform("canvasSize", glm::vec2(m_maskFBO->GetWidth(), m_maskFBO->GetHeight()));

	return true;
}

bool OutlineEffect::Render(RenderContext* context, Framebuffer* mainFBO, ScreenMesh* screenMesh)
{
	// 0. 데이터 확인
	if (!context || !mainFBO) return false;
	const auto& outlines = context->GetMeshOutlines();
	if (outlines.empty()) return true;

	// [Pass 1] 마스크 텍스처 생성 (Mask Generation)
	m_maskFBO->Bind();

	// 1. 화면 클리어 (투명한 검은색)
	glViewport(0, 0, m_maskFBO->GetWidth(), m_maskFBO->GetHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Always On Top 효과를 위해 깊이 설정 조정
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// 마스킹 렌더링 (단색으로 그리기)
	MaskMeshes(outlines);

	glDepthFunc(GL_LESS);

	// 다시 메인 화면(디폴트 FBO)으로 복귀
	mainFBO->Bind();
	glViewport(0, 0, mainFBO->GetWidth(), mainFBO->GetHeight());

	// [Pass 2] 포스트 프로세싱 합성
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_postProgram->Use();

	// 0번 슬롯에 마스크 텍스처 바인딩
	glActiveTexture(GL_TEXTURE0);
	m_maskFBO->GetTexture()->Bind();

	// 아웃라인 색상 및 두께 전달
	m_postProgram->SetUniform("outlineSize", m_thickness);
	m_postProgram->SetUniform("outlineColor", glm::vec4(m_color, 1.0f));

	// ScreenMesh로 화면 꽉 차게 그리기
	screenMesh->Draw();

	// 상태 복구
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	return true;
}

/*==============================//
//   outlining helper methods   //
//==============================*/
void OutlineEffect::MaskMeshes(const std::vector<MeshOutline*>& outlines)
{
	// 1. 분류를 위한 정적 벡터
	static std::vector<MeshOutline*> staticOutlines;  staticOutlines.clear();
	static std::vector<MeshOutline*> skinnedOutlines; skinnedOutlines.clear();

	// 2. 전체 리스트는 여기서 딱 한 번만 순회
	for (auto* outline : outlines)
	{
		// 1. 기본 유효성 검사 (여기서 미리 다 걸러냅니다)
		if (!outline->IsEnabled()) continue;
		if (!outline->GetOwner()->IsActive()) continue;

		// 2. 캐싱된 렌더러 가져오기
		MeshRenderer* renderer = outline->GetMeshRenderer();
		if (!renderer || !renderer->IsEnabled()) continue;

		// 3. MeshRenderer의 타입에 맞춰서 분리
		switch (renderer->GetComponentType())
		{
		case ComponentType::StaticMeshRenderer:
		{
			staticOutlines.push_back(outline);
			break;
		}

		case ComponentType::SkinnedMeshRenderer:
		{
			skinnedOutlines.push_back(outline);
			break;
		}

		// TODO : InstancedMeshRenderer에 대해서도 처리 필요
		default:
			break;
		}
	}

	// 4. 분류된 리스트를 넘겨서 그리기
	if (!staticOutlines.empty()) MaskStaticMeshes(staticOutlines);
	if (!skinnedOutlines.empty()) MaskSkinnedMeshes(skinnedOutlines);
}

void OutlineEffect::MaskStaticMeshes(const std::vector<MeshOutline*>& outlines)
{
	m_maskStaticProgram->Use();

	for (auto* outline : outlines)
	{
		// 정적 매쉬 렌더러 가져오기
		MeshRenderer* renderer = outline->GetMeshRenderer();
		auto* staticRenderer = static_cast<StaticMeshRenderer*>(renderer);

		// 유니폼 설정
		m_maskStaticProgram->SetUniform("model", staticRenderer->GetTransform().GetWorldMatrix());
		m_maskStaticProgram->SetUniform("outlineColor", outline->GetColor());

		renderer->Render(m_maskStaticProgram.get());
	}
}

void OutlineEffect::MaskSkinnedMeshes(const std::vector<MeshOutline*>& outlines)
{
	m_maskSkinnedProgram->Use();

	// 이미 검증된 Skinned 목록만 들어옴
	for (auto* outline : outlines)
	{
		// 스킨드 매쉬 렌더러 가져오기
		MeshRenderer* renderer = outline->GetMeshRenderer();
		auto* skinnedRenderer = static_cast<SkinnedMeshRenderer*>(renderer);

		// 유니폼 설정
		m_maskSkinnedProgram->SetUniform("model", renderer->GetTransform().GetWorldMatrix());
		m_maskSkinnedProgram->SetUniform("outlineColor", outline->GetColor());

		// 애니메이터 처리
		Animator* animator = skinnedRenderer->GetAnimator();
		const auto& boneMatrices = (animator && animator->IsEnabled() && animator->GetOwner()->IsActive())
			? animator->GetFinalBoneMatrices()
			: Utils::GetIdentityBones();

		m_maskSkinnedProgram->SetUniform("finalBoneMatrices", boneMatrices);

		renderer->Render(m_maskSkinnedProgram.get());
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
