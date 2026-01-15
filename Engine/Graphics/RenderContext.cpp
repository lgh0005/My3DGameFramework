#include "EnginePch.h"
#include "RenderContext.h"
#include "Graphics/SkyLight.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/Light.h"
#include "Components/MeshOutline.h"
#include "Resources/CubeTexture.h"
#include "Resources/Texture.h"
#include "Scene/Scene.h"
#include "Scene/ComponentRegistry.h"
#include "Object/Component.h"

DECLARE_DEFAULTS_IMPL(RenderContext)

/*====================================//
//   default render context methods   //
//====================================*/
void RenderContext::Reset(Scene* scene, Camera* camera)
{
	// 1. 기본 정보 갱신
	m_currentSceneRegistry = scene->GetRegistry();
	m_currentCamera = camera;

	// 2. 씬의 원본 데이터 연결
	if (m_currentSceneRegistry)
	{
		m_staticMeshRenderersSrc  = &m_currentSceneRegistry->GetComponents<StaticMeshRenderer>();
		m_skinnedMeshRenderersSrc = &m_currentSceneRegistry->GetComponents<SkinnedMeshRenderer>();
		m_lightsSrc				  = &m_currentSceneRegistry->GetComponents<Light>();
		m_skyLight			      = m_currentSceneRegistry->GetSkyLight();
	}
	else
	{
		m_staticMeshRenderersSrc = nullptr;
		m_skinnedMeshRenderersSrc = nullptr;
		m_lightsSrc = nullptr;
		m_skyLight = nullptr;
	}

	// 3. 결과 벡터 초기화
	m_culledMeshOutlines.clear();
	m_culledStaticMeshRenderers.clear();
	m_culledSkinnedMeshRenderers.clear();
	m_culledLights.clear();
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
//   default render context getters   //
//====================================*/
ComponentRegistry* RenderContext::GetSceneRegistry() const
{
	return m_currentSceneRegistry;
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

const RenderContext::ComponentVectorRawPtr RenderContext::GetSourceLights() const
{
	return m_lightsSrc;
}