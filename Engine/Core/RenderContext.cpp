#include "EnginePch.h"
#include "RenderContext.h"
#include "Graphics/SkyLight.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"

RenderContext::~RenderContext() = default;

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
