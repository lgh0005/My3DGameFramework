#include "EnginePch.h"
#include "RenderContext.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"

void RenderContext::Reset(Scene* scene, Camera* camera,
							   int32 width, int32 height)
{
	// 현재 Tick에서의 주요 정보들 갱신
	m_currentScene = scene;
	m_currentCamera = camera;
	m_currentWidth = width;
	m_currentHeight = height;

	// [최적화] 메모리 해제 없이 size만 0으로 만듦 (Capacity 유지)
	m_staticMeshRenderers.clear();
	m_skinnedMeshRenderers.clear();
	m_lights.clear();
}

void RenderContext::AddStaticMeshRenderer(MeshRenderer* renderer)
{
	m_staticMeshRenderers.push_back(renderer);
}

void RenderContext::AddSkinnedMeshRenderer(MeshRenderer* renderer)
{
	m_skinnedMeshRenderers.push_back(renderer);
}

void RenderContext::AddLight(Light* light)
{
	m_lights.push_back(light);
}