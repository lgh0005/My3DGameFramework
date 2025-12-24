#include "EnginePch.h"
#include "RenderContext.h"
#include "Graphics/SkyLight.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"
#include "Resources/CubeTexture.h"
#include "Resources/Texture.h"

RenderContext::~RenderContext() = default;

/*====================================//
//   default render context methods   //
//====================================*/
void RenderContext::Reset(Scene* scene, Camera* camera)
{
	// 1. 기본 정보 갱신
	m_currentScene  = scene;
	m_currentCamera = camera;

	// 2. 씬의 원본 데이터 연결
	if (m_currentScene)
	{
		m_staticMeshRenderers  = &scene->GetStaticMeshRenderers();
		m_skinnedMeshRenderers = &scene->GetSkinnedMeshRenderers();
		m_lights			   = &scene->GetLights();
		m_skyLight			   = m_currentScene->GetSkyLight();
	}

	// 3. 결과 벡터 초기화
	m_culledStaticMeshRenderers.clear();
	m_culledSkinnedMeshRenderers.clear();
	m_culledLights.clear();
}

void RenderContext::AddStaticMeshRenderer(MeshRenderer* renderer)
{
	m_culledStaticMeshRenderers.push_back(renderer);
}

void RenderContext::AddSkinnedMeshRenderer(MeshRenderer* renderer)
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
Scene* RenderContext::GetScene() const
{
	return m_currentScene;
}

Camera* RenderContext::GetCamera() const
{
	return m_currentCamera;
}

SkyLight* RenderContext::GetSkyLight() const
{
	return m_skyLight;
}

const std::vector<MeshRenderer*>& RenderContext::GetStaticMeshRenderers() const
{
	return m_culledStaticMeshRenderers;
}

const std::vector<MeshRenderer*>& RenderContext::GetSkinnedMeshRenderers() const
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