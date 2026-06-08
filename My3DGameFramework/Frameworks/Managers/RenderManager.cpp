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
#include "Window/MGFWindow.h"

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

	void RenderManager::Resize(int32 width, int32 height)
	{
		// 창이 최소화(0x0)되었을 때 그래픽스 API 오류(Crash) 방지
		if (width <= 0 || height <= 0) return;

		// 1. 현재 해상도 캐싱
		m_currentWidth = width;
		m_currentHeight = height;

		// 2. 전체 OpenGL 뷰포트 크기 갱신
		glViewport(0, 0, width, height);

		// 3. RenderContext의 공통 '도화지' 크기 갱신
		// G-Buffer와 포스트 프로세싱 핑퐁 버퍼가 새 해상도에 맞춰 다시 할당됩니다.
		if (m_renderContext)
		{
			m_renderContext->InitGeometryBuffer(width, height);
			m_renderContext->InitPostProcessBuffer(width, height);
		}

		// 4. [핵심] 씬에 존재하는 모든 카메라의 종횡비(Aspect Ratio) 동기화
		// 이 부분이 화면 찌그러짐(Stretching)을 막아주는 핵심 로직입니다.
		auto* cameraRegistry = MGF_ENTITY.GetComponentRegistry<Camera>();
		if (cameraRegistry)
		{
			for (auto* camera : cameraRegistry->GetComponents())
			{
				// 카메라 컴포넌트의 비율을 변경하고 투영 행렬(Projection Matrix)을 재계산합니다.
				camera->SetViewportSize(static_cast<float>(width), static_cast<float>(height));
			}
		}
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
			Resize(m_currentWidth, m_currentHeight);
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