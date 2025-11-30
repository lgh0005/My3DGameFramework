#include "EnginePch.h"
#include "StandardGeometryPass.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/FrameBuffer.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"

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
		"./Resources/Shaders/DeferredShading/gBuffer_static.vert",
		"./Resources/Shaders/DeferredShading/gBuffer.frag"
	);
	m_skinnedGeometryProgram = Program::Create
	(
		"./Resources/Shaders/DeferredShading/gBuffer_skinned.vert",
		"./Resources/Shaders/DeferredShading/gBuffer.frag"
	);
	if (!m_staticGeometryProgram || !m_skinnedGeometryProgram) return false;

	m_gBuffer = Framebuffer::CreateGBuffer(width, height);
	if (!m_gBuffer) return false;

	return true;
}

// TODO : Render 추상 메서드 생김새를 조금 다듬을 필요는 있음
void StandardGeometryPass::Render(Scene* scene, Camera* camera)
{
	// 1. G-Buffer FBO 바인딩
	m_gBuffer->Bind();

	// 2. 화면 클리어
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, m_gBuffer->GetWidth(), m_gBuffer->GetHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// 3. Static Mesh 그리기 (정적 오브젝트)
	if (m_staticGeometryProgram)
	{
		m_staticGeometryProgram->Use();

		for (const auto* renderer : m_staticMeshRenderers)
		{
			MeshPtr mesh = renderer->GetMesh();
			auto model = renderer->GetTransform().GetModelMatrix();
			auto material = renderer->GetMaterial();

			material->SetToProgram(m_staticGeometryProgram.get());
			m_staticGeometryProgram->SetUniform("model", model);

			mesh->Draw(m_staticGeometryProgram.get());
		}
	}

	// 4. Skinned Mesh 그리기 (애니메이션 오브젝트)
	if (m_skinnedGeometryProgram)
	{
		m_skinnedGeometryProgram->Use();

		for (const auto* renderer : m_skinnedMeshRenderers)
		{
			GameObject* go = renderer->GetOwner();
			MeshPtr mesh = renderer->GetMesh();
			auto material = renderer->GetMaterial();
			auto& transform = go->GetTransform();
			Animator* animator = go->GetComponent<Animator>();

			material->SetToProgram(m_skinnedGeometryProgram.get());

			auto finalMatrices = animator->GetFinalBoneMatrices();
			for (int i = 0; i < finalMatrices.size(); ++i)
				m_skinnedGeometryProgram->SetUniform("finalBoneMatrices[" + std::to_string(i) + "]", finalMatrices[i]);
			m_skinnedGeometryProgram->SetUniform("model", transform.GetModelMatrix());

			mesh->Draw(m_skinnedGeometryProgram.get());
		}
	}

	// 5. 그리기 완료 후 기본 프레임버퍼로 복귀
	Framebuffer::BindToDefault();
}

void StandardGeometryPass::Resize(int32 width, int32 height)
{
	m_gBuffer = Framebuffer::CreateGBuffer(width, height);
}

/*================================================//
//   standard geometry pass getters and setters   //
//================================================*/
void StandardGeometryPass::AddStaticMeshRenderer(MeshRenderer* staticMeshRenderer)
{
	m_staticMeshRenderers.push_back(staticMeshRenderer);
}

void StandardGeometryPass::AddSkinnedMeshRenderer(MeshRenderer* skinnedMeshRenderer)
{
	m_skinnedMeshRenderers.push_back(skinnedMeshRenderer);
}

const std::vector<MeshRenderer*>& StandardGeometryPass::GetSkinnedMeshRenderers() const
{
	return m_skinnedMeshRenderers;
}

const std::vector<MeshRenderer*>& StandardGeometryPass::GetStaticMeshRenderers() const
{
	return m_staticMeshRenderers;
}