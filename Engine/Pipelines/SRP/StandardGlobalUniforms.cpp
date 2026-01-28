#include "EnginePch.h"
#include "Object/GameObject.h"
#include "StandardGlobalUniforms.h"
#include "Graphics/Buffers/UniformBuffer.h"
#include "Graphics/Rendering/RenderContext.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/Light.h"
#include "Components/DirectionalLight.h"
#include "Components/SpotLight.h"
#include "Components/PointLight.h"

DECLARE_DEFAULTS_IMPL(StandardGlobalUniforms)

StandardGlobalUniformsUPtr StandardGlobalUniforms::Create()
{
	auto globalUniforms = StandardGlobalUniformsUPtr(new StandardGlobalUniforms());
	if (!globalUniforms->Init()) return nullptr;
	return std::move(globalUniforms);
}

bool StandardGlobalUniforms::Init()
{
	m_cameraUBO = UniformBuffer::Create(sizeof(CameraData));
	if (m_cameraUBO) m_cameraUBO->BindBase(UBO_POINT_CAMERA);

	m_lightUBO = UniformBuffer::Create(sizeof(LightData));
	if (m_lightUBO) m_lightUBO->BindBase(UBO_POINT_LIGHT);

	return (m_cameraUBO && m_lightUBO);
}

void StandardGlobalUniforms::PreRender(RenderContext* context)
{
	if (!context) return;

	// 0. context로부터 필요한 정보 얻어오기
	auto camera = context->GetCamera();
	auto lights = context->GetLights();

	if (!camera) return;

	// 1. 카메라 데이터 업데이트
	UpdateCameraUBO(camera);

	// 2. 조명 데이터 업데이트 (Specular 계산을 위해 ViewPos 필요)
	UpdateLightUBO(lights, camera->GetTransform().GetWorldPosition());
}

/*==============================//
//   UBO update helper methods  //
//==============================*/
void StandardGlobalUniforms::UpdateCameraUBO(Camera* camera)
{
	if (!m_cameraUBO || !camera) return;

	CameraData camData;
	camData.view = camera->GetViewMatrix();
	camData.projection = camera->GetProjectionMatrix();
	camData.viewPos = camera->GetTransform().GetWorldPosition();

	m_cameraUBO->SetData(&camData, sizeof(CameraData));
}

void StandardGlobalUniforms::UpdateLightUBO
(
	const std::vector<Light*>& lights, 
	const glm::vec3& viewPos
)
{
	if (!m_lightUBO) return;

	LightData lightData = {};
	lightData.viewPos = viewPos;

	int32 lightCount = 0;
	int32 shadowCasterCount = 0;

	for (auto* light : lights)
	{
		if (lightCount >= MAX_LIGHTS) break;

		if (!light->IsEnabled()) continue;
		if (!light->GetOwner()->IsActive()) continue;

		auto& info = lightData.lights[lightCount];
		auto& transform = light->GetTransform();

		// [공통 속성]
		info.position = transform.GetWorldPosition();
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
		switch (light->GetComponentType())
		{
			case ComponentType::DirectionalLight:
			{
				info.type = 0;
				info.attenuation = glm::vec3(1.0f, 0.0f, 0.0f);
				info.cutoff = glm::vec2(0.0f, 0.0f);
				break;
			}

			case ComponentType::PointLight:
			{
				info.type = 1;
				auto point = static_cast<PointLight*>(light);
				info.attenuation = point->GetAttenuation();
				info.cutoff = glm::vec2(0.0f, 0.0f);
				break;
			}

			case ComponentType::SpotLight:
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