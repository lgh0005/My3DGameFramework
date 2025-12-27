#include "EnginePch.h"
#include "OutlinePass.h"

// 필수 헤더 포함
#include "Core/RenderContext.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Resources/Mesh.h"
#include "Components/MeshRenderer.h"
#include "Components/MeshOutline.h"
#include "Components/Animator.h"
#include "Components/Transform.h"

OutlinePass::OutlinePass() = default;
OutlinePass::~OutlinePass() = default;

OutlinePassUPtr OutlinePass::Create()
{
	auto pass = OutlinePassUPtr(new OutlinePass());
	if (!pass->Init()) return nullptr;
	return std::move(pass);
}

bool OutlinePass::Init()
{
	// 1. Static용 프로그램 로드
	m_staticProgram = Program::Create
	(
		"./Resources/Shaders/Common/Common_Outline_Static.vert",
		"./Resources/Shaders/Common/Common_Outline.frag"
	);

	// 2. Skinned용 프로그램 로드
	m_skinnedProgram = Program::Create
	(
		"./Resources/Shaders/Common/Common_Outline_Skinned.vert",
		"./Resources/Shaders/Common/Common_Outline.frag"
	);

	// 둘 다 성공해야 true
	return (m_staticProgram && m_skinnedProgram);
}

void OutlinePass::Render(RenderContext* context)
{
	// 1. 그릴 대상이 없으면 빠른 리턴
	const auto& outlines = context->GetMeshOutlines();
	if (outlines.empty()) return;

	// 2. 공통 상태 설정
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	//// [Pass 1] 마스킹 (Masking)
	MaskMeshes(outlines);

	//// [Pass 2] 그리기 (Drawing)
	DrawMeshes(outlines);

	// [Cleanup] 상태 복구
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
}

/*==============================//
//   outlining helper methods   //
//==============================*/
void OutlinePass::MaskMeshes(const std::vector<MeshOutline*>& outlines)
{
	// [Pass 1] 마스킹 (Masking)
	glStencilFunc(GL_ALWAYS, 1, 0xFF);         // 항상 통과
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // 통과 시 1로 교체
	glStencilMask(0xFF);                       // 쓰기 허용

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // 색상 끄기
	glDepthMask(GL_FALSE);                               // 깊이 끄기

	MaskStaticMeshes(outlines);
	MaskSkinnedMeshes(outlines);
}

void OutlinePass::DrawMeshes(const std::vector<MeshOutline*>& outlines)
{
	// [Pass 2] 그리기 (Drawing)
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // 1이 아닌 곳만 통과
	glStencilMask(0x00);                 // 스텐실 읽기 전용으로 전환

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // 색상 켜기
	glDisable(GL_DEPTH_TEST);                        // X-Ray 효과 (선택사항)

	// 헬퍼 호출 (그리기)
	DrawStaticMeshOutlines(outlines);
	DrawSkinnedMeshOutlines(outlines);
}

void OutlinePass::MaskStaticMeshes(const std::vector<MeshOutline*>& outlines)
{
	m_staticProgram->Use();
	m_staticProgram->SetUniform("outlineThickness", 0.0f);

	for (auto* outline : outlines)
	{
		auto* owner = outline->GetOwner();

		// Animator가 있으면 Static 패스가 아니므로 스킵
		if (owner->GetComponent<Animator>()) continue;

		// [심플한 해결책] "내 몸에 붙은 렌더러 내놔"
		auto* renderer = owner->GetComponent<MeshRenderer>();
		auto mesh = renderer->GetMesh();
		m_staticProgram->SetUniform("model", renderer->GetTransform().GetWorldMatrix());
		mesh->Draw(m_staticProgram.get());
	}
}

void OutlinePass::MaskSkinnedMeshes(const std::vector<MeshOutline*>& outlines)
{
	m_skinnedProgram->Use();
	m_skinnedProgram->SetUniform("outlineThickness", 0.0f); // 마스킹은 두께 0

	for (auto* outline : outlines)
	{
		auto* owner = outline->GetOwner();
		auto* animator = owner->GetComponent<Animator>();
		if (!animator) continue;
		auto* renderer = owner->GetComponent<MeshRenderer>();
		auto mesh = renderer->GetMesh();

		m_skinnedProgram->SetUniform("model", renderer->GetTransform().GetWorldMatrix());
		const auto& finalMatrices = animator->GetFinalBoneMatrices();
		for (int i = 0; i < finalMatrices.size(); ++i)
			m_skinnedProgram->SetUniform("finalBoneMatrices[" + std::to_string(i) + "]", finalMatrices[i]);

		mesh->Draw(m_skinnedProgram.get());
	}
}

void OutlinePass::DrawStaticMeshOutlines(const std::vector<MeshOutline*>& outlines)
{
	m_staticProgram->Use();

	for (auto* outline : outlines)
	{
		auto* owner = outline->GetOwner();
		if (owner->GetComponent<Animator>()) continue;

		// [Pass 2] 실제 두께와 색상 적용
		m_staticProgram->SetUniform("outlineColor", outline->GetColor());
		m_staticProgram->SetUniform("outlineThickness", outline->GetThickness());

		auto* renderer = owner->GetComponent<MeshRenderer>();
		auto mesh = renderer->GetMesh();
		m_staticProgram->SetUniform("model", renderer->GetTransform().GetWorldMatrix());
		mesh->Draw(m_staticProgram.get());
	}
}

void OutlinePass::DrawSkinnedMeshOutlines(const std::vector<MeshOutline*>& outlines)
{
	m_skinnedProgram->Use();

	for (auto* outline : outlines)
	{
		auto* owner = outline->GetOwner();
		auto* animator = owner->GetComponent<Animator>();
		if (!animator) continue;

		// [Pass 2] 실제 두께와 색상 적용
		m_skinnedProgram->SetUniform("outlineColor", outline->GetColor());
		m_skinnedProgram->SetUniform("outlineThickness", outline->GetThickness());

		auto* renderer = owner->GetComponent<MeshRenderer>();
		auto mesh = renderer->GetMesh();
		m_skinnedProgram->SetUniform("model", renderer->GetTransform().GetWorldMatrix());

		const auto& finalMatrices = animator->GetFinalBoneMatrices();
		for (int i = 0; i < finalMatrices.size(); ++i)
			m_skinnedProgram->SetUniform("finalBoneMatrices[" + std::to_string(i) + "]", finalMatrices[i]);
		mesh->Draw(m_skinnedProgram.get());
	}
}
