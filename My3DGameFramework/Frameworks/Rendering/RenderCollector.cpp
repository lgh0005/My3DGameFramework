#include "FrameworkPch.h"
#include "RenderCollector.h"
#include "Rendering/RenderContext.h"
#include "Managers/EntityManager.h"
#include "Managers/InstanceManager.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Lights/SpotLight.h"

namespace MGF3D
{
	RenderCollector::RenderCollector() = default;
	RenderCollector::~RenderCollector() = default;

	void RenderCollector::Clear()
	{
		m_dirLights.clear();
		m_pointLights.clear();
		m_spotLights.clear();

		m_dirShadows.clear();
		m_pointShadows.clear();
		m_spotShadows.clear();
	}

	void RenderCollector::CollectCameras(RenderContext* context, const Camera* camera)
	{
		if (!camera || !context) return;

		CameraData camData;
		camData.view = camera->GetViewMatrix();
		camData.projection = camera->GetProjectionMatrix();

		Transform* camTransform = MGF_ENTITY.GetComponent<Transform>(camera->GetOwnerID());
		camData.viewPos = camTransform->GetWorldPosition();
		camData.pad0 = 0.0f;

		context->UpdateCameras(camData);
	}

	void RenderCollector::CollectMeshData(RenderContext* context)
	{
		MGF_INSTANCE.Extract(context);
	}

	void RenderCollector::CollectDirectionalLights(RenderContext* context)
	{
		if (!context) return;

		auto dirLightRegsitry = MGF_ENTITY.GetComponentRegistry<DirectionalLight>();
		for (auto* light : dirLightRegsitry->GetComponents())
		{
			if (m_dirLights.size() >= MAX_LIGHTS) break;

			DirectionalLightData data;
			Transform* transform = MGF_ENTITY.GetComponent<Transform>(light->GetOwnerID());

			// 조명의 방향
			data.direction = vec4(transform->GetForward(), 0.0f);
			data.color = vec4(light->GetColor(), light->GetIntensity());
			data.shadowIndex = -1;

			// 그림자 데이터 수집
			if (light->IsCastShadow() && m_dirShadows.size() < MAX_SHADOW_CASTER)
			{
				DirectionalShadowData sData;

				// TODO: 여기서 카메라 프러스트럼 기반 CSM 행렬 계산 로직 호출
				// CalculateCSMMatrices(sData, camera);
				data.shadowIndex = static_cast<int32>(m_dirShadows.size());
				m_dirShadows.push_back(sData);
			}

			m_dirLights.push_back(data);
		}

		context->UpdateDirectionalLights(m_dirLights);
		context->UpdateDirectionalShadows(m_dirShadows);
	}

	void RenderCollector::CollectPointLights(RenderContext* context)
	{
		if (!context) return;

		auto pointLightRegsitry = MGF_ENTITY.GetComponentRegistry<PointLight>();
		for (auto* light : pointLightRegsitry->GetComponents())
		{
			if (m_pointLights.size() >= MAX_LIGHTS) break;

			PointLightData data;
			Transform* transform = MGF_ENTITY.GetComponent<Transform>(light->GetOwnerID());

			// 위치 및 범위 (w 성분에 Range 저장)
			data.position = vec4(transform->GetWorldPosition(), light->GetRange());
			data.color = vec4(light->GetColor(), light->GetIntensity());
			data.shadowIndex = -1;

			// 그림자 데이터 수집
			if (light->IsCastShadow() && m_pointShadows.size() < MAX_SHADOW_CASTER)
			{
				PointShadowData sData;
				sData.shadowFarPlane = light->GetRange();
				sData.shadowMapIdx = static_cast<int32>(m_pointShadows.size());
				sData.shadowBias = 0.05f; // 기본 바이어스 값

				data.shadowIndex = static_cast<int32>(m_pointShadows.size());
				m_pointShadows.push_back(sData);
			}

			m_pointLights.push_back(data);
		}

		context->UpdatePointLights(m_pointLights);
		context->UpdatePointShadows(m_pointShadows);
	}

	void RenderCollector::CollectSpotLights(RenderContext* context)
	{
		if (!context) return;

		auto spotLightRegistry = MGF_ENTITY.GetComponentRegistry<SpotLight>();
		for (auto* light : spotLightRegistry->GetComponents())
		{
			if (m_spotLights.size() >= MAX_LIGHTS) break;

			SpotLightData data;
			Transform* transform = MGF_ENTITY.GetComponent<Transform>(light->GetOwnerID());

			// 위치/범위 및 방향
			data.position = vec4(transform->GetWorldPosition(), light->GetRange());
			data.direction = vec4(transform->GetForward(), 0.0f);
			data.color = vec4(light->GetColor(), light->GetIntensity());

			// 조명 파라미터 (x: InnerCutoff, y: OuterCutoff)
			data.params = vec4(light->GetInnerCutoff(), light->GetOuterCutoff(), 0.0f, 0.0f);
			data.shadowIndex = -1;

			// 그림자 데이터 수집
			if (light->IsCastShadow() && m_spotShadows.size() < MAX_SHADOW_CASTER)
			{
				SpotShadowData sData;

				// TODO: 스팟 라이트용 View-Projection 행렬 계산 로직 필요
				// sData.lightSpaceMatrix = CalculateSpotVP(transform, light);
				sData.shadowMapIdx = static_cast<int32>(m_spotShadows.size());
				sData.shadowBias = 0.005f;

				data.shadowIndex = static_cast<int32>(m_spotShadows.size());
				m_spotShadows.push_back(sData);
			}

			m_spotLights.push_back(data);
		}

		context->UpdateSpotLights(m_spotLights);
		context->UpdateSpotShadows(m_spotShadows);
	}
}