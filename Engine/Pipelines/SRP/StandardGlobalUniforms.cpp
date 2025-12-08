#include "EnginePch.h"
#include "StandardGlobalUniforms.h"
#include "Graphics/UniformBuffer.h"
#include "Core/RenderContext.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/Light.h"
#include "Components/DirectionalLight.h"
#include "Components/SpotLight.h"
#include "Components/PointLight.h"

StandardGlobalUniforms::StandardGlobalUniforms() = default;
StandardGlobalUniforms::~StandardGlobalUniforms() = default;

StandardGlobalUniformsUPtr StandardGlobalUniforms::Create()
{
	auto globalUniforms = StandardGlobalUniformsUPtr(new StandardGlobalUniforms());
	if (!globalUniforms->Init()) return nullptr;
	return std::move(globalUniforms);
}

bool StandardGlobalUniforms::Init()
{
	m_cameraUBO = Uniformbuffer::Create(sizeof(CameraData), UBO_POINT_CAMERA);
	m_lightUBO = Uniformbuffer::Create(sizeof(LightData), UBO_POINT_LIGHT);
	return (m_cameraUBO && m_lightUBO);
}

void StandardGlobalUniforms::PreRender(RenderContext* context)
{
	// 0. context로부터 필요한 정보 얻어오기
	auto camera = context->GetCamera();
	auto lights = context->GetLights();

	// 1. 카메라 UBO
	if (m_cameraUBO)
	{
		CameraData camData;
		camData.view = camera->GetViewMatrix();
		camData.projection = camera->GetProjectionMatrix();
		camData.viewPos = camera->GetTransform().GetPosition();
		m_cameraUBO->SetData(&camData, sizeof(CameraData));
	}

	// 2. 조명 UBO
	if (m_lightUBO)
	{
		LightData lightData = {};
		lightData.viewPos = camera->GetTransform().GetPosition();
		int32 lightCount = 0;
		int32 shadowCasterCount = 0;

		// 조명 데이터 주입 분류
		for (auto* light : lights)
		{
			if (lightCount >= MAX_LIGHTS) break;

			auto& info = lightData.lights[lightCount];
			auto& transform = light->GetTransform();

			// [공통 속성]
			info.position = transform.GetPosition();
			info.direction = transform.GetForwardVector();
			info.ambient = light->GetAmbient();
			info.diffuse = light->GetDiffuse();
			info.specular = light->GetSpecular();
			info.intensity = light->GetIntensity();

			// [그림자 캐스팅 필요 판정]
			if (light->IsCastShadow() && shadowCasterCount < MAX_SHADOW_CASTER)
			{
				info.shadowMapIndex = shadowCasterCount;
				light->SetShadowMapIndex(shadowCasterCount);
				shadowCasterCount++;
			}
			else
			{
				info.shadowMapIndex = -1;
				light->SetShadowMapIndex(-1);
			}

			// [타입별 속성]
			switch (light->GetLightType())
			{
				case LightType::Directional:
				{
					info.type = 0;
					info.attenuation = glm::vec3(1.0f, 0.0f, 0.0f);
					info.cutoff = glm::vec2(0.0f, 0.0f);
					break;
				}

				case LightType::Point:
				{
					info.type = 1;
					auto point = static_cast<PointLight*>(light);
					info.attenuation = point->GetAttenuation();
					info.cutoff = glm::vec2(0.0f, 0.0f);
					break;
				}

				case LightType::Spot:
				{
					info.type = 2;
					auto spot = static_cast<SpotLight*>(light);
					glm::vec2 cutoff = spot->GetCutoff();
					info.attenuation = spot->GetAttenuation();
					info.cutoff.x = cosf(glm::radians(cutoff[0]));
					info.cutoff.y = cosf(glm::radians(cutoff[0] + cutoff[1]));
					break;
				}
			}
			lightCount++;
		}

		lightData.lightCount = lightCount;
		m_lightUBO->SetData(&lightData, sizeof(LightData));
	}
}