#include "EnginePch.h"
#include "UniversalGeometryPass.h"

#include "Scene/Scene.h"
#include "Object/GameObject.h"
#include "Graphics/Rendering/RenderContext.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"
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

DECLARE_DEFAULTS_IMPL(UniversalGeometryPass)

UniversalGeometryPassUPtr UniversalGeometryPass::Create(int32 width, int32 height)
{
	auto pass = UniversalGeometryPassUPtr(new UniversalGeometryPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool UniversalGeometryPass::Init(int32 width, int32 height)
{
	m_staticGeometryProgram = RESOURCE.GetResource<GraphicsProgram>("universal_deferred_geometry_static");
	m_skinnedGeometryProgram = RESOURCE.GetResource<GraphicsProgram>("universal_deferred_geometry_skinned");
	m_gBuffer = GBufferFramebuffer::Create(width, height);
	if (!m_gBuffer) return false;
	return true;
}

void UniversalGeometryPass::Render(RenderContext* context)
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

	// 5. context에 gBuffer 캐싱
	context->SetGBuffer(m_gBuffer.get());

	// 6. 그리기 완료 후 기본 프레임버퍼로 복귀
	Framebuffer::BindToDefault();
}

void UniversalGeometryPass::RenderStaticGeometry(const std::vector<StaticMeshRenderer*>& meshes, const glm::mat4& vp)
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

void UniversalGeometryPass::RenderSkinnedGeometry(const std::vector<SkinnedMeshRenderer*>& meshes, const glm::mat4& vp)
{
	if (!m_skinnedGeometryProgram || meshes.empty()) return;

	m_skinnedGeometryProgram->Use();
	for (const auto* renderer : meshes)
	{
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

void UniversalGeometryPass::Resize(int32 width, int32 height)
{
	m_gBuffer = GBufferFramebuffer::Create(width, height);
}

