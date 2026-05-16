#include "FrameworkPch.h"
#include "RenderManager.h"
#include "Managers/WindowManager.h"
#include "Managers/EntityManager.h"
#include "Managers/InstanceManager.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/RenderContext.h"
#include "Rendering/RenderCollector.h"
#include "Components/Camera.h"
#include "Components/Lights/SkyLight.h"
#include "Graphics/Framebuffers/GLFramebufferHandle.h"
#include "Graphics/Framebuffers/GLFramebuffer2D.h"

namespace MGF3D
{
	RenderManager::RenderManager() = default;
	RenderManager::~RenderManager() = default;

	bool RenderManager::Init()
	{
		m_renderContext = MakeUnique<RenderContext>();
		m_renderCollector = MakeUnique<RenderCollector>();
		if (!m_renderContext || !m_renderCollector) return false;
		return true;
	}

	void RenderManager::Render()
	{
		if (!m_activePipeline || !m_renderContext || !m_renderCollector) return;

		// 1. 카메라 컴포넌트 레지스트리 가져오기
		auto* cameraRegistry = MGF_ENTITY.GetComponentRegistry<Camera>();
		if (!cameraRegistry) return;

		// 2. 하늘 컴포넌트 레지스트리 가져오기
		auto* skyLightRegistry = MGF_ENTITY.GetComponentRegistry<SkyLight>();
		if (!skyLightRegistry) return;

		// 2. 렌더링 데이터 수집기 초기화
		m_renderCollector->Clear();
		
		// 2. 조명 데이터 수집
		m_renderCollector->CollectDirectionalLights(m_renderContext.get());
		m_renderCollector->CollectPointLights(m_renderContext.get());
		m_renderCollector->CollectSpotLights(m_renderContext.get());
		auto& skies = skyLightRegistry->GetComponents();
		const SkyLight* mainSkyLight = skies.IsEmpty() ? nullptr : skies[0];

		// 3. 카메라 순회
		const auto& cameras = cameraRegistry->GetComponents();
		for (const auto* camera : cameras)
		{
			// 3-1. 현재 카메라의 렌더링 데이터 수집
			m_renderCollector->CollectGlobals(m_renderContext.get(), camera, mainSkyLight);

			// 3-2. 씬의 엔티티들로부터 메쉬 및 인스턴스 데이터 추출
			m_renderCollector->CollectMeshData(m_renderContext.get(), camera);

			// 3-3. 파이프라인 실행
			m_activePipeline->Render(m_renderContext.get());
			GLFramebufferHandle::Unbind();

			// 3-4. 다음 카메라(혹은 프레임)를 위한 큐 정리
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

	uint32 RenderManager::GetFinalSceneTextureID() const
	{
		if (!m_renderContext) return 0;
		
		GLTexture2D* finalTexture = m_renderContext->GetSceneColorTexture();
		if (!finalTexture) return 0;
		
		return finalTexture->GetHandle();
	}
}