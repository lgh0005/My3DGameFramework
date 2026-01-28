#include "EnginePch.h"
#include "StandardGeometryPass.h"

#include "Scene/Scene.h"
#include "Object/GameObject.h"
#include "Graphics/Rendering/RenderContext.h"
#include "Resources/Program.h"
#include "Resources/Meshes/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Textures/Texture.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"
#include "Graphics/Framebuffers/GBufferFramebuffer.h"

DECLARE_DEFAULTS_IMPL(StandardGeometryPass)

StandardGeometryPassUPtr StandardGeometryPass::Create(int32 width, int32 height)
{
	auto pass = StandardGeometryPassUPtr(new StandardGeometryPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool StandardGeometryPass::Init(int32 width, int32 height)
{
	m_staticGeometryProgram = RESOURCE.GetResource<Program>("standard_deferred_geometry_static");
	m_skinnedGeometryProgram = RESOURCE.GetResource<Program>("standard_deferred_geometry_skinned");
	m_gBuffer = GBufferFramebuffer::Create(width, height);
	if (!m_gBuffer) return false;
	return true;
}

void StandardGeometryPass::Render(RenderContext* context)
{
	// 0. 자신의 렌더 패스에 활용되고 있는 RenderContext로 캐스팅
	Camera* camera = context->GetCamera();
	float shutterSpeed = RENDER.GetRenderer()->GetTargetShutterSpeed();
	glm::mat4 virtualPrevVP = glm::mat4(1.0f);
	if (camera) virtualPrevVP = camera->GetVirtualPrevViewProjectionMatrix(shutterSpeed);

	// 1. G-Buffer FBO 바인딩
	m_gBuffer->Bind();

	// 2. 화면 클리어
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, m_gBuffer->GetWidth(), m_gBuffer->GetHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// 3. Static Mesh 그리기 (정적 오브젝트)
	RenderStaticGeometry(context->GetStaticMeshRenderers(), virtualPrevVP);
		
	// 4. Skinned Mesh 그리기 (애니메이션 오브젝트)
	RenderSkinnedGeometry(context->GetSkinnedMeshRenderers(), virtualPrevVP);

	// 4. context에 gBuffer 캐싱
	context->SetGBuffer(m_gBuffer.get());

	// 5. 그리기 완료 후 기본 프레임버퍼로 복귀
	Framebuffer::BindToDefault();
}

void StandardGeometryPass::RenderStaticGeometry(const std::vector<StaticMeshRenderer*>& meshes, const glm::mat4& vp)
{
	if (!m_staticGeometryProgram || meshes.empty()) return;

	m_staticGeometryProgram->Use();
	for (const auto* renderer : meshes)
	{
		if (!renderer->IsEnabled()) continue;
		if (!renderer->GetOwner()->IsActive()) continue;

		auto model = renderer->GetTransform().GetWorldMatrix();
		m_staticGeometryProgram->SetUniform("model", model);

		glm::mat4 prevMVP = vp * model;
		m_staticGeometryProgram->SetUniform("uPrevModelViewProj", prevMVP);

		renderer->Render(m_staticGeometryProgram.get());
	}
}

void StandardGeometryPass::RenderSkinnedGeometry(const std::vector<SkinnedMeshRenderer*>& meshes, const glm::mat4& vp)
{
	if (!m_skinnedGeometryProgram || meshes.empty()) return;

	m_skinnedGeometryProgram->Use();
	for (const auto* renderer : meshes)
	{
		// TODO : 이후에 CullingPass와 파티셔닝을 통한 O(N) 렌더링에서 O(k) 렌더링으로
		// 최적화가 가능하며 순수하게 이러한 파티셔닝 정렬은 멀티스레딩을 이용할 수도 있다.
		if (!renderer->IsEnabled()) continue;
		if (!renderer->GetOwner()->IsActive()) continue;

		auto model = renderer->GetTransform().GetWorldMatrix();
		m_skinnedGeometryProgram->SetUniform("model", model);

		glm::mat4 prevMVP = vp * model;
		m_skinnedGeometryProgram->SetUniform("uPrevModelViewProj", prevMVP);

		Animator* animator = renderer->GetAnimator();
		if (animator) m_skinnedGeometryProgram->SetUniform("finalBoneMatrices", animator->GetFinalBoneMatrices());
		else m_skinnedGeometryProgram->SetUniform("finalBoneMatrices", GetIdentityBones());
	
		renderer->Render(m_skinnedGeometryProgram.get());
	}
}

void StandardGeometryPass::Resize(int32 width, int32 height)
{
	m_gBuffer->OnResize(width, height);
}
