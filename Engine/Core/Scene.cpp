#include "EnginePch.h"
#include "Scene.h"

#include "Core/GameObject.h"
#include "Graphics/UniformBuffer.h"
#include "Components/Component.h"
#include "Components/Light.h"
#include "Components/MeshRenderer.h"
#include "Components/Camera.h"
#include "Components/Animator.h"
#include "Components/Script.h"
#include "Components/Transform.h"

Scene::Scene() = default;
Scene::~Scene() = default;

void Scene::AddGameObject(GameObjectUPtr gameObject)
{
	auto* go = gameObject.get();
	for (const auto& comp : go->GetAllComponents())
		RegisterComponent(comp.get());

	m_gameObjects.push_back(std::move(gameObject));
}

void Scene::AddRenderPass(const std::string& name, RenderPassUPtr renderPass)
{
	auto it = m_renderPasses.find(name);
	if (it != m_renderPasses.end())
		SPDLOG_WARN("RenderPass '{}' already exists. Overwriting.", name);
	m_renderPasses[name] = std::move(renderPass);
}

void Scene::RegisterComponent(Component* component)
{
	if (!component) return;

	// TODO : 이후에 다른 게임 컴포넌트들도 나열해야함
	switch (component->GetType())
	{
	case ComponentType::Camera:
	{
		m_cameras.push_back(static_cast<Camera*>(component));
		break;
	}
	case ComponentType::Light:
	{
		m_lights.push_back(static_cast<Light*>(component));
		break;
	}
	case ComponentType::Animator:
	{
		m_animators.push_back(static_cast<Animator*>(component));
		break;
	}
	case ComponentType::Script:
	{
		m_scripts.push_back(static_cast<Script*>(component));
		break;
	}
	default: return;
	}
}

void Scene::PreRender()
{
	// 1. 카메라 UBO 값 갱신
	if (!m_mainCamera || !m_cameraUBO) return;

	CameraData camData;
	camData.view = m_mainCamera->GetViewMatrix();
	camData.projection = m_mainCamera->GetProjectionMatrix();
	camData.viewPos = m_mainCamera->GetTransform().GetPosition();
	m_cameraUBO->SetData(&camData, sizeof(CameraData));

	// TODO : 다른 UBO도 값 갱신 필요

	// 3. 그림자 UBO 값 갱신
	if (!m_shadowPass) return;
	ShadowData shadowData;
	shadowData.lightSpaceMatrix = m_shadowPass->GetLightSpaceMatrix();
	m_shadowUBO->SetData(&shadowData, sizeof(ShadowData));
}

void Scene::OnScreenResize(int32 width, int32 height)
{
	auto* camera = GetMainCamera();
	if (camera) camera->SetAspectRatio((float)width / (float)height);

	if (m_geometryPass) m_geometryPass->Resize(width, height);
	if (m_postProcessPass) m_postProcessPass->Resize(width, height);
}

bool Scene::Init()
{
	// TODO : UBO 테스트 중. 이후에 SRP로 같이 넘어가야 함.
	auto result = CreateRenderUBOs();
	if (result != 0)
	{
		SPDLOG_ERROR("failed to create render ubo. {}", result);
		return false;
	}

	if (!LoadNessesaryResources())
	{
		SPDLOG_ERROR("failed to load resources.");
		return false;
	}

	if (!CreateNessesaryRenderPasses())
	{
		SPDLOG_ERROR("failed to create render passes.");
		return false;
	}

	if (!CreateSceneContext())
	{
		SPDLOG_ERROR("failed to create scene context.");
		return false;
	}

	return true;
}

// TEMP : UBO 테스트 중
int32 Scene::CreateRenderUBOs()
{
	m_cameraUBO = Uniformbuffer::Create(sizeof(CameraData), UBO_POINT_CAMERA);
	if (!m_cameraUBO) return 1;

	m_lightUBO = Uniformbuffer::Create(sizeof(LightData), UBO_POINT_LIGHT);
	if (!m_lightUBO) return 2;

	m_shadowUBO = Uniformbuffer::Create(sizeof(ShadowData), UBO_POINT_SHADOW);
	if (!m_shadowUBO) return 3;

	return 0;
}

void Scene::Start()
{
	// TODO : 이후에 컴포넌트가 필연적으로 Start가 필요하게 된다면
	// 컴포넌트에 추가적인 가상 함수를 둘 필요가 있다.
	// 특히 Awake, LateUpdate, FixedUpdate 등
	// 2. 스크립트 컴포넌트 Start
	for (auto* script : m_scripts)
	{
		script->Start();
	}
}

void Scene::Update()
{
	// TODO : 
	// 0. 나중에 추가할 Destroy/SetActive 로직을 고려

	// TODO
	// 1. 캐시된 m_animators 목록을 순회하며 애니메이션 업데이트
	for (auto* animator : m_animators)
	{
		// (나중에 추가) if (animator->GetOwner()->IsActiveInHierarchy())

		// 기존 Context::Render()에 있던 애니메이션 업데이트 로직을
		// Scene::Update()로 이동
		animator->UpdateAnimation();
	}
	
	// 2. 스크립트 컴포넌트 업데이트
	for (auto* script : m_scripts)
	{
	     script->Update();
	}

	// 3. 파괴가 예약된 오브젝트들 일괄 삭제
	FlushDestroyQueue();
}

void Scene::FlushDestroyQueue()
{
	// TODO : 씬에 Destroy를 호출한 오브젝트를 정리
}

/*============================================//
//   essential render pass getter & setters   //
//============================================*/
RenderPass* Scene::GetRenderPass(const std::string& name)
{
	auto it = m_renderPasses.find(name);
	if (it != m_renderPasses.end()) return it->second.get();
	return nullptr;
}