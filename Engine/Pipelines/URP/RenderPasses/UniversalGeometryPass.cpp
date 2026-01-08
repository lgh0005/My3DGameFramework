#include "EnginePch.h"
#include "UniversalGeometryPass.h"

#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Graphics/RenderContext.h"
#include "Resources/Program.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Texture.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"
#include "Framebuffers/GBufferFramebuffer.h"

#include "Pipelines/URP/UniversalRenderPipeline.h"
#include "Pipelines/URP/UniversalRenderContext.h"

DECLARE_DEFAULTS_IMPL(UniversalGeometryPass)

UniversalGeometryPassUPtr UniversalGeometryPass::Create(int32 width, int32 height)
{
	auto pass = UniversalGeometryPassUPtr(new UniversalGeometryPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool UniversalGeometryPass::Init(int32 width, int32 height)
{
	m_staticGeometryProgram = RESOURCE.GetResource<Program>("universal_deferred_geometry_static");
	m_skinnedGeometryProgram = RESOURCE.GetResource<Program>("universal_deferred_geometry_skinned");
	m_gBuffer = GBufferFramebuffer::Create(width, height);
	if (!m_gBuffer) return false;
	return true;
}

void UniversalGeometryPass::Render(RenderContext* context)
{
	// 0. 자신의 렌더 패스에 활용되고 있는 RenderContext로 캐스팅
	auto stdCtx = (UniversalRenderContext*)context;

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

	// 5. context에 gBuffer 캐싱
	stdCtx->SetGBuffer(m_gBuffer.get());

	// 6. 그리기 완료 후 기본 프레임버퍼로 복귀
	Framebuffer::BindToDefault();
}

void UniversalGeometryPass::RenderStaticGeometry(const std::vector<StaticMeshRenderer*>& meshes)
{
	if (!m_staticGeometryProgram || meshes.empty()) return;

	m_staticGeometryProgram->Use();
	for (const auto* renderer : meshes)
	{
		if (!renderer->IsEnabled()) continue;
		if (!renderer->GetOwner()->IsActiveInHierarchy()) continue;

		auto model = renderer->GetTransform().GetWorldMatrix();
		m_staticGeometryProgram->SetUniform("model", model);
		renderer->Render(m_staticGeometryProgram.get());
	}
}

void UniversalGeometryPass::RenderSkinnedGeometry(const std::vector<SkinnedMeshRenderer*>& meshes)
{
	if (!m_skinnedGeometryProgram || meshes.empty()) return;

	m_skinnedGeometryProgram->Use();
	for (const auto* renderer : meshes)
	{
		if (!renderer->IsEnabled()) continue;
		if (!renderer->GetOwner()->IsActiveInHierarchy()) continue;

		auto model = renderer->GetTransform().GetWorldMatrix();
		m_skinnedGeometryProgram->SetUniform("model", model);

		Animator* animator = renderer->GetAnimator();
		if (animator) m_skinnedGeometryProgram->SetUniform("finalBoneMatrices", animator->GetFinalBoneMatrices());
		else m_skinnedGeometryProgram->SetUniform("finalBoneMatrices", GetIdentityBones());

		renderer->Render(m_skinnedGeometryProgram.get());
	}
}

void UniversalGeometryPass::Resize(int32 width, int32 height)
{
	m_gBuffer = GBufferFramebuffer::Create(width, height);
}

/*=================================================//
//   universal geometry pass getters and setters   //
//=================================================*/
void UniversalGeometryPass::AddStaticMeshRenderer(StaticMeshRenderer* staticMeshRenderer)
{
	m_staticMeshRenderers.push_back(staticMeshRenderer);
}

void UniversalGeometryPass::AddSkinnedMeshRenderer(SkinnedMeshRenderer* skinnedMeshRenderer)
{
	m_skinnedMeshRenderers.push_back(skinnedMeshRenderer);
}

const std::vector<StaticMeshRenderer*>& UniversalGeometryPass::GetStaticMeshRenderers() const
{
	return m_staticMeshRenderers;
}

const std::vector<SkinnedMeshRenderer*>& UniversalGeometryPass::GetSkinnedMeshRenderers() const
{
	return m_skinnedMeshRenderers;
}
