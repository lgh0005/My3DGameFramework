#include "EnginePch.h"
#include "StandardGeometryPass.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Core/RenderContext.h"
#include "Graphics/Program.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Texture.h"
#include "Components/MeshRenderer.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"
#include "Framebuffers/GBufferFramebuffer.h"

#include "Pipelines/SRP/StandardRenderPipeline.h"
#include "Pipelines/SRP/StandardRenderContext.h"

StandardGeometryPass::StandardGeometryPass() = default;
StandardGeometryPass::~StandardGeometryPass() = default;

StandardGeometryPassUPtr StandardGeometryPass::Create(int32 width, int32 height)
{
	auto pass = StandardGeometryPassUPtr(new StandardGeometryPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool StandardGeometryPass::Init(int32 width, int32 height)
{
	m_staticGeometryProgram = Program::Create
	(
		"./Resources/Shaders/Standard/Standard_Deferred_GeometryPass_Static.vert",
		"./Resources/Shaders/Standard/Standard_Deferred_GeometryPass.frag"
	);
	m_skinnedGeometryProgram = Program::Create
	(
		"./Resources/Shaders/Standard/Standard_Deferred_GeometryPass_Skinned.vert",
		"./Resources/Shaders/Standard/Standard_Deferred_GeometryPass.frag"
	);
	if (!m_staticGeometryProgram || !m_skinnedGeometryProgram) return false;

	m_gBuffer = GBufferFramebuffer::Create(width, height);
	if (!m_gBuffer) return false;

	return true;
}

void StandardGeometryPass::Render(RenderContext* context)
{
	// 0. 자신의 렌더 패스에 활용되고 있는 RenderContext로 캐스팅
	auto stdCtx = (StandardRenderContext*)context;

	// 1. G-Buffer FBO 바인딩
	m_gBuffer->Bind();

	// 2. 화면 클리어
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, m_gBuffer->GetWidth(), m_gBuffer->GetHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// 3. Static Mesh 그리기 (정적 오브젝트)
	RenderStaticGeometry(stdCtx->GetStaticMeshRenderers());
		
	// 4. Skinned Mesh 그리기 (애니메이션 오브젝트)
	RenderSkinnedGeometry(stdCtx->GetSkinnedMeshRenderers());

	// 4. context에 gBuffer 캐싱
	stdCtx->SetGBuffer(m_gBuffer.get());

	// 5. 그리기 완료 후 기본 프레임버퍼로 복귀
	Framebuffer::BindToDefault();
}

void StandardGeometryPass::RenderStaticGeometry(const std::vector<StaticMeshRenderer*>& meshes)
{
	if (!m_staticGeometryProgram || meshes.empty()) return;

	m_staticGeometryProgram->Use();
	for (const auto* renderer : meshes)
	{
		auto model = renderer->GetTransform().GetWorldMatrix();
		m_staticGeometryProgram->SetUniform("model", model);
		renderer->Render(m_staticGeometryProgram.get());
	}
}

void StandardGeometryPass::RenderSkinnedGeometry(const std::vector<SkinnedMeshRenderer*>& meshes)
{
	if (!m_skinnedGeometryProgram || meshes.empty()) return;

	m_skinnedGeometryProgram->Use();
	for (const auto* renderer : meshes)
	{
		auto model = renderer->GetTransform().GetWorldMatrix();
		m_skinnedGeometryProgram->SetUniform("model", model);

		Animator* animator = renderer->GetAnimator();
		if (animator) m_skinnedGeometryProgram->SetUniform("finalBoneMatrices", animator->GetFinalBoneMatrices());
		else m_skinnedGeometryProgram->SetUniform("finalBoneMatrices", GetIdentityBones());
	
		renderer->Render(m_skinnedGeometryProgram.get());
	}
}

void StandardGeometryPass::Resize(int32 width, int32 height)
{
	m_gBuffer = GBufferFramebuffer::Create(width, height);
}

/*================================================//
//   standard geometry pass getters and setters   //
//================================================*/
void StandardGeometryPass::AddStaticMeshRenderer(StaticMeshRenderer* staticMeshRenderer)
{
	m_staticMeshRenderers.push_back(staticMeshRenderer);
}

void StandardGeometryPass::AddSkinnedMeshRenderer(SkinnedMeshRenderer* skinnedMeshRenderer)
{
	m_skinnedMeshRenderers.push_back(skinnedMeshRenderer);
}

const std::vector<StaticMeshRenderer*>& StandardGeometryPass::GetStaticMeshRenderers() const
{
	return m_staticMeshRenderers;
}

const std::vector<SkinnedMeshRenderer*>& StandardGeometryPass::GetSkinnedMeshRenderers() const
{
	return m_skinnedMeshRenderers;
}
