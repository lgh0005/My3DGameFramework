#include "EnginePch.h"
#include "Scene.h"

#include "Core/GameObject.h"
#include "Graphics/UniformBuffer.h"
#include "Components/Component.h"
#include "Components/Light.h"
#include "Components/PointLight.h"
#include "Components/SpotLight.h"
#include "Components/DirectionalLight.h"
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

void Scene::OnScreenResize(int32 width, int32 height)
{
	// TODO : 이후에는 다중 카메라에 대해서 모든 카메라가 리사이징 되어야함
	auto* camera = GetMainCamera();
	if (camera) camera->SetAspectRatio((float)width / (float)height);
	// TODO : SRP가 Viewport를 책임짐
}

bool Scene::Init()
{
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

void Scene::Start()
{
	// TODO : 이후에 컴포넌트가 필연적으로 Start가 필요하게 된다면
	// 컴포넌트에 추가적인 가상 함수를 둘 필요가 있는지 고려 필요.
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