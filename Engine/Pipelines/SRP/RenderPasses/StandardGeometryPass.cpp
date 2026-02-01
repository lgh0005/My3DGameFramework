#include "EnginePch.h"
#include "StandardGeometryPass.h"

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
#include "Instancing/RenderQueue.h"

DECLARE_DEFAULTS_IMPL(StandardGeometryPass)

StandardGeometryPassUPtr StandardGeometryPass::Create(int32 width, int32 height)
{
	auto pass = StandardGeometryPassUPtr(new StandardGeometryPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool StandardGeometryPass::Init(int32 width, int32 height)
{
	m_geometryInstancedProgram = RESOURCE.GetResource<GraphicsProgram>("standard_deferred_geometry");
	m_renderQueue = RenderQueue::Create(1024, 512 * MAX_BONES);
	m_gBuffer = GBufferFramebuffer::Create(width, height);
	if (!m_gBuffer || !m_geometryInstancedProgram || !m_gBuffer) return false;
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

	// 3. 모든 메쉬들을 읽어서 GBuffer에 기록
	RenderGeometry(context, virtualPrevVP);

	// 4. context에 gBuffer 캐싱
	context->SetGBuffer(m_gBuffer.get());

	// 5. 그리기 완료 후 기본 프레임버퍼로 복귀
	Framebuffer::BindToDefault();
}

void StandardGeometryPass::RenderGeometry(RenderContext* context, const glm::mat4& prevVP)
{
	if (!m_geometryInstancedProgram) return;

	// 1. 큐 초기화 및 셰이더 준비
	m_renderQueue->Clear();
	m_geometryInstancedProgram->Use();
	m_geometryInstancedProgram->SetUniform("uPrevVP", prevVP);

	// 2. Static Mesh 데이터 수집
	for (const auto* renderer : context->GetStaticMeshRenderers())
	{
		if (!renderer->IsEnabled() || !renderer->GetOwner()->IsActive()) continue;

		InstanceProperty prop;
		prop.worldMatrix = renderer->GetTransform().GetWorldMatrix();
		prop.color = renderer->GetColor();
		m_renderQueue->Add(renderer->GetMesh().get(), renderer->GetMaterial().get(), prop);
	}

	// 3. Skinned Mesh 데이터 수집
	for (const auto* renderer : context->GetSkinnedMeshRenderers())
	{
		if (!renderer->IsEnabled() || !renderer->GetOwner()->IsActive()) continue;

		InstanceProperty prop;
		prop.worldMatrix = renderer->GetTransform().GetWorldMatrix();
		prop.color = renderer->GetColor();

		Animator* animator = renderer->GetAnimator();
		const auto& bones = animator ? animator->GetFinalBoneMatrices() : Utils::GetIdentityBones();
		m_renderQueue->Add(renderer->GetMesh().get(), renderer->GetMaterial().get(), prop, bones);
	}

	// 4. 단 한 번의 실행으로 모든 Batch를 순회하며 렌더링
	m_renderQueue->Execute(m_geometryInstancedProgram.get());
}

void StandardGeometryPass::Resize(int32 width, int32 height)
{
	m_gBuffer->OnResize(width, height);
}