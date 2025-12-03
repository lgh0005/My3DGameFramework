#include "EnginePch.h"
#include "RenderContext.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"

RenderContext::~RenderContext() = default;

void RenderContext::Reset(Scene* scene, Camera* camera)
{
	// 1. 기본 정보 갱신
	m_currentScene = scene;
	m_currentCamera = camera;

	// 2. 씬의 원본 데이터 연결
	m_staticMeshRenderers  = &scene->GetStaticMeshRenderers();
	m_skinnedMeshRenderers = &scene->GetSkinnedMeshRenderers();
	m_lights			   = &scene->GetLights();

	// 3. 결과 벡터 초기화
	m_culledStaticMeshRenderers.clear();
	m_culledSkinnedMeshRenderers.clear();
	m_culledLights.clear();
}