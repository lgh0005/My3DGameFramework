#include "EnginePch.h"
#include "RenderContext.h"
#include "Components/SkyLight.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/Light.h"
#include "Components/MeshOutline.h"
#include "Resources/Textures/CubeTexture.h"
#include "Resources/Textures/Texture.h"
#include "Scene/Scene.h"
#include "Scene/SceneRegistry.h"
#include "Scene/ComponentRegistry.h"
#include "Object/Component.h"
#include "Graphics/Framebuffers/Framebuffer.h"
#include "Graphics/Framebuffers/GBufferFramebuffer.h"

RenderContext::RenderContext()
{
	m_textures.fill(nullptr);
}
RenderContext::~RenderContext() = default;

/*====================================//
//   default render context methods   //
//====================================*/
void RenderContext::Reset(Scene* scene, Camera* camera)
{
	// 1. 기본 정보 갱신
	m_componentRegistry = scene->GetComponentRegistry();
	m_currentCamera = camera;

	// 2. 씬의 원본 데이터 연결
	if (m_componentRegistry)
	{
		m_staticMeshRenderersSrc  = &m_componentRegistry->GetComponents<StaticMeshRenderer>();
		m_skinnedMeshRenderersSrc = &m_componentRegistry->GetComponents<SkinnedMeshRenderer>();
		m_meshOutlinesSrc		  = &m_componentRegistry->GetComponents<MeshOutline>();
		m_lightsSrc				  = &m_componentRegistry->GetComponents<Light>();
		m_skyLight			      = m_componentRegistry->GetSkyLight(0); // TODO : 임시처방. 이거 따로 빼야할 수 있음
	}
	else
	{
		m_staticMeshRenderersSrc  = nullptr;
		m_skinnedMeshRenderersSrc = nullptr;
		m_meshOutlinesSrc		  = nullptr;
		m_lightsSrc				  = nullptr;
		m_skyLight				  = nullptr;
	}

	// 3. 결과 벡터 초기화
	m_culledStaticMeshRenderers.clear();
	m_culledSkinnedMeshRenderers.clear();
	m_culledMeshOutlines.clear();
	m_culledLights.clear();

	// 4. 자원 및 타겟 초기화 (매 프레임 깨끗하게 시작)
	m_textures.fill(nullptr);
	m_targetFramebuffer = nullptr;
	for (int i = 0; i < MAX_SHADOW_CASTER; ++i) m_shadowMaps[i] = nullptr;
	if (m_skyLight) m_skyboxTexture = m_skyLight->GetSkybox();
}

void RenderContext::AddMeshOutline(MeshOutline* outline)
{
	m_culledMeshOutlines.push_back(outline);
}

void RenderContext::AddStaticMeshRenderer(StaticMeshRenderer* renderer)
{
	m_culledStaticMeshRenderers.push_back(renderer);
}

void RenderContext::AddSkinnedMeshRenderer(SkinnedMeshRenderer* renderer)
{
	m_culledSkinnedMeshRenderers.push_back(renderer);
}

void RenderContext::AddLight(Light* light)
{
	m_culledLights.push_back(light);
}

/*====================================//
//    Resource & Target Management    //
//====================================*/
void RenderContext::SetGBuffer(GBufferFramebuffer* gBuffer)
{
	if (gBuffer)
	{
		// G-Buffer의 각 텍스처를 표준 슬롯에 연결
		SetTexture(RenderSlot::GPosition, gBuffer->GetColorAttachment(0).get());
		SetTexture(RenderSlot::GNormal, gBuffer->GetColorAttachment(1).get());
		SetTexture(RenderSlot::GAlbedo, gBuffer->GetColorAttachment(2).get());
		SetTexture(RenderSlot::GEmission, gBuffer->GetColorAttachment(3).get());
		SetTexture(RenderSlot::GVelocity, gBuffer->GetColorAttachment(4).get());
	}
	else
	{
		// null이 들어오면 관련 슬롯 비우기
		SetTexture(RenderSlot::GPosition, nullptr);
		SetTexture(RenderSlot::GNormal, nullptr);
		SetTexture(RenderSlot::GAlbedo, nullptr);
		SetTexture(RenderSlot::GEmission, nullptr);
		SetTexture(RenderSlot::GVelocity, nullptr);
	}
}

void RenderContext::BindTargetFramebuffer() const
{
	if (m_targetFramebuffer)
	{
		// 지정된 타겟(FBO) 바인딩 및 뷰포트 설정
		m_targetFramebuffer->Bind();
		glViewport(0, 0, m_targetFramebuffer->GetWidth(), m_targetFramebuffer->GetHeight());
	}
	else
	{
		// 타겟이 없으면 기본 백버퍼(화면) 바인딩
		Framebuffer::BindToDefault();
		glViewport(0, 0, WINDOW.GetWindowWidth(), WINDOW.GetWindowHeight());
	}

	// [중요] 렌더 패스 간 상태 오염 방지를 위한 기본 상태 강제 설정
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

/*====================================//
//   default render context getters   //
//====================================*/
ComponentRegistry* RenderContext::GetSceneRegistry() const
{
	return m_componentRegistry;
}

Camera* RenderContext::GetCamera() const
{
	return m_currentCamera;
}

SkyLight* RenderContext::GetSkyLight() const
{
	return m_skyLight;
}

const std::vector<MeshOutline*>& RenderContext::GetMeshOutlines() const
{
	return m_culledMeshOutlines;
}

const std::vector<StaticMeshRenderer*>& RenderContext::GetStaticMeshRenderers() const
{
	return m_culledStaticMeshRenderers;
}

const std::vector<SkinnedMeshRenderer*>& RenderContext::GetSkinnedMeshRenderers() const
{
	return m_culledSkinnedMeshRenderers;
}

const std::vector<Light*>& RenderContext::GetLights() const
{
	return m_culledLights;
}

/*======================================================//
//   common resource methods for common render passes   //
//======================================================*/
void RenderContext::SetShadowMap(int32 index, Texture* texture)
{
	if (index >= 0 && index < MAX_SHADOW_CASTER) m_shadowMaps[index] = texture;
}

Texture* RenderContext::GetShadowMap(int32 index) const
{
	if (index >= 0 && index < MAX_SHADOW_CASTER) return m_shadowMaps[index];
	return nullptr;
}

void RenderContext::SetSkyboxTexture(CubeTexture* texture)
{
	m_skyboxTexture = texture;
}

CubeTexture* RenderContext::GetSkyboxTexture() const
{
	return m_skyboxTexture;
}

const RenderContext::ComponentVectorRawPtr RenderContext::GetSourceStaticMeshes() const
{
	return m_staticMeshRenderersSrc;
}

const RenderContext::ComponentVectorRawPtr RenderContext::GetSourceSkinnedMeshes() const
{
	return m_skinnedMeshRenderersSrc;
}

const RenderContext::ComponentVectorRawPtr RenderContext::GetSourceMeshOutlines() const
{
	return m_meshOutlinesSrc;
}

const RenderContext::ComponentVectorRawPtr RenderContext::GetSourceLights() const
{
	return m_lightsSrc;
}