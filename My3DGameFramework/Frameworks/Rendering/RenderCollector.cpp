#include "FrameworkPch.h"
#include "RenderCollector.h"
#include "Rendering/RenderContext.h"
#include "Managers/EntityManager.h"
#include "Managers/InstanceManager.h"
#include "Managers/TimeManager.h"
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

	void RenderCollector::CollectGlobals(RenderContext* context, const Camera* camera)
	{
		if (!camera || !context) return;

		GlobalData globalData;

		// 1. 카메라 데이터 수집
		globalData.view = camera->GetViewMatrix();
		globalData.projection = camera->GetProjectionMatrix();
		Transform* camTransform = MGF_ENTITY.GetComponent<Transform>(camera->GetOwnerID());
		globalData.viewPos = camTransform->GetWorldPosition();

		// 2. 시간 데이터 수집
		globalData.time = static_cast<float>(MGF_TIME.GetTime());

		// 3. 조명 개수 수집
		globalData.dirLightCount = static_cast<int32>(m_dirLights.size());
		globalData.pointLightCount = static_cast<int32>(m_pointLights.size());
		globalData.spotLightCount = static_cast<int32>(m_spotLights.size());
		globalData.pad0 = 0;

		context->UpdateGlobals(globalData);
		context->SetCurrentCamera(camera);
	}

	void RenderCollector::CollectMeshData(RenderContext* context, const Camera* camera)
	{
		MGF_INSTANCE.Extract(context, camera);
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
				sData.shadowMapBaseIdx = static_cast<int32>(m_dirShadows.size()) * MAX_DIR_SHADOW_LAYERS;
				sData.shadowBias = 0.00005f;
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
				sData.shadowBias = 0.05f;

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
			float innerCos = Math::Cos(Math::ToRadians(light->GetInnerCutoff()));
			float outerCos = Math::Cos(Math::ToRadians(light->GetOuterCutoff()));
			data.params = vec4(innerCos, outerCos, 0.0f, 0.0f);
			data.shadowIndex = -1;

			// 그림자 데이터 수집
			if (light->IsCastShadow() && m_spotShadows.size() < MAX_SHADOW_CASTER)
			{
				SpotShadowData sData;

				// 1. Projection: 빛의 퍼짐 각도(OuterCutoff의 2배)를 FOV로 사용합니다.
				float fov = Math::ToRadians(light->GetOuterCutoff() * 2.0f);
				mat4 lightProj = glm::perspective(fov, 1.0f, 0.1f, light->GetRange());

				// 2. View: 조명의 위치에서 조명의 방향을 바라보는 행렬을 만듭니다.
				vec3 lightPos = transform->GetWorldPosition();
				vec3 lightDir = transform->GetForward();
				vec3 up = (Math::Abs(lightDir.y) > 0.999f) ? vec3(0.0f, 0.0f, 1.0f) : vec3(0.0f, 1.0f, 0.0f);
				mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, up);

				sData.lightSpaceMatrix = lightProj * lightView;
				sData.shadowMapIdx = static_cast<int32>(m_spotShadows.size());
				sData.shadowBias = 0.00005f;

				data.shadowIndex = static_cast<int32>(m_spotShadows.size());
				m_spotShadows.push_back(sData);
			}

			m_spotLights.push_back(data);
		}

		context->UpdateSpotLights(m_spotLights);
		context->UpdateSpotShadows(m_spotShadows);
	}
}