#include "EnginePch.h"
#include "StandardOutlinePass.h"

#include "Graphics/RenderContext.h"
#include "Object/GameObject.h"
#include "Resources/Program.h"
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

#include "Pipelines/SRP/StandardRenderContext.h"
#include "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h"

DECLARE_DEFAULTS_IMPL(StandardOutlinePass)

StandardOutlinePassUPtr StandardOutlinePass::Create
(
	int32 width, int32 height, 
	const glm::vec3& color,
	float thickness)
{
	auto pass = StandardOutlinePassUPtr(new StandardOutlinePass());
	if (!pass->Init(width, height, color, thickness)) return nullptr;
	return std::move(pass);
}

bool StandardOutlinePass::Init
(
	int32 width, int32 height, 
	const glm::vec3& color,
	float thickness
)
{
	m_color = color;
	m_thickness = thickness;

	m_maskStaticProgram = RESOURCE.GetResource<Program>("common_outline_static");
	m_maskSkinnedProgram = RESOURCE.GetResource<Program>("common_outline_skinned");
	m_postProgram = RESOURCE.GetResource<Program>("common_outline_postprocess");
	m_screenMesh = RESOURCE.GetResource<ScreenMesh>("Screen");
	m_maskFBO = OutlineFramebuffer::Create(width, height);
	if (!m_maskStaticProgram || !m_maskSkinnedProgram || !m_postProgram || !m_screenMesh || !m_maskFBO)
		return false;

	// 변하지 않는 유니폼은 Init에서 설정
	m_postProgram->Use();
	m_postProgram->SetUniform("stencilTexture", 0);
	m_postProgram->SetUniform("outlineColor", glm::vec4(m_color, 1.0f));

	return true;
}

void StandardOutlinePass::Render(RenderContext* context)
{
	// 0. 캐스팅 및 데이터 확인
	auto stdCtx = (StandardRenderContext*)context;
	const auto& outlines = context->GetMeshOutlines();
	if (outlines.empty()) return;

	// [Pass 1] 마스크 텍스처 생성 (Mask Generation)
	m_maskFBO->Bind();

	// 1. 화면 클리어 (투명한 검은색)
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
		auto* tPos = stdCtx->GetGBufferPosition();
		glViewport(0, 0, tPos->GetWidth(), tPos->GetHeight());
	}

	// [Pass 2] 포스트 프로세싱 합성
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_postProgram->Use();

	// 0번 슬롯에 마스크 텍스처 바인딩
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_maskFBO->GetTexture()->Get());

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

void StandardOutlinePass::Resize(int32 width, int32 height)
{
	m_maskFBO = OutlineFramebuffer::Create(width, height);
}

/*==============================//
//   outlining helper methods   //
//==============================*/
void StandardOutlinePass::MaskMeshes(const std::vector<MeshOutline*>& outlines)
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

void StandardOutlinePass::MaskStaticMeshes(const std::vector<MeshOutline*>& outlines)
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

void StandardOutlinePass::MaskSkinnedMeshes(const std::vector<MeshOutline*>& outlines)
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
			: GetIdentityBones();

		m_maskSkinnedProgram->SetUniform("finalBoneMatrices", boneMatrices);

		renderer->Render(m_maskSkinnedProgram.get());
	}
}
