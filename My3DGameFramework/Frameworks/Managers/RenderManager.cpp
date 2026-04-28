#include "FrameworkPch.h"
#include "RenderManager.h"
#include "Managers/WindowManager.h"
#include "Managers/EntityManager.h"
#include "Managers/InstanceManager.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/RenderContext.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Uniforms/CameraUniform.h"

namespace MGF3D
{
	RenderManager::RenderManager() = default;
	RenderManager::~RenderManager() = default;

	bool RenderManager::Init()
	{
		m_renderContext = MakeUnique<RenderContext>();
		if (!m_renderContext) return false;
		return true;
	}

	void RenderManager::Render()
	{
		if (!m_activePipeline || !m_renderContext) return;

		// 1. 카메라 컴포넌트 레지스트리 가져오기
		auto* cameraRegistry = MGF_ENTITY.GetComponentRegistry<Camera>();
		if (!cameraRegistry) return;
		
		const auto& cameras = cameraRegistry->GetComponents();
		for (const auto* camera : cameras)
		{
			// 2. 현재 카메라의 렌더링 데이터 조립 및 업데이트
			CameraData camData;
			camData.view = camera->GetViewMatrix();
			camData.projection = camera->GetProjectionMatrix();

			Transform* camTransform = MGF_ENTITY.GetComponent<Transform>(camera->GetOwnerID());
			if (camTransform) camData.viewPos = camTransform->GetWorldPosition();
			else camData.viewPos = vec3(0.0f, 0.0f, 0.0f);

			camData.pad0 = 0.0f;

			m_renderContext->UpdateCameras(camData);

			// 3. 씬의 엔티티들로부터 데이터 추출 (핵심)
			MGF_INSTANCE.Extract(m_renderContext.get());

			// 4. 파이프라인 실행
			m_activePipeline->Render(m_renderContext.get());

			// 5. 다음 프레임을 위한 큐 정리
			m_renderContext->ClearQueues();
		}
	}

	void RenderManager::Resize()
	{
		m_activePipeline->Resize();
	}

	void RenderManager::Shutdown()
	{
		m_activePipeline.reset();
		m_pipelines.clear();
	}

	RenderPipeline* RenderManager::GetActiveRenderPipeline() const
	{
		return m_activePipeline.get();
	}

	void RenderManager::SetRenderPipeline(StringView name)
	{
		auto it = m_pipelines.find(StringHash(name));
		if (it != m_pipelines.end())
		{
			m_activePipeline = it->second();
			Resize();
		}
	}
}