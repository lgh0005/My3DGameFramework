#include "EnginePch.h"
#include "Scene.h"

#include "Scene/GameObject.h"
#include "Graphics/RenderPass.h"
#include "Resources/Mesh.h"
#include "Graphics/SkyLight.h"
#include "Components/Component.h"
#include "Components/Light.h"
#include "Components/PointLight.h"
#include "Components/DirectionalLight.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/InstancedMeshRenderer.h"
#include "Components/MeshOutline.h"
#include "Components/Camera.h"
#include "Components/Animator.h"
#include "Components/Script.h"
#include "Components/Transform.h"
#include "Components/AudioSource.h"
#include "Components/AudioListener.h"
#include "UIs/UICanvas.h"
#include "GameObjectManager.h"

Scene::Scene() = default;
Scene::~Scene() = default;

/*==============================================//
//   default scene context management methods   //
//==============================================*/
void Scene::AddGameObject(GameObjectUPtr gameObject)
{
	auto* go = gameObject.get();
	for (const auto& comp : go->GetAllComponents())
		RegisterComponent(comp.get());

	m_gameObjects.push_back(std::move(gameObject));
}

void Scene::OnScreenResize(int32 width, int32 height)
{
	// TODO : 이후에는 다중 카메라에 대해서 모든 카메라가 리사이징 되어야함
	auto* camera = GetMainCamera();
	if (camera) camera->SetViewportSize((float)width, (float)height);
}

/*===================================================================//
//   object, component and custom render passes management methods   //
//===================================================================*/
void Scene::RegisterComponent(Component* component)
{
	if (!component) return;

	// TODO : 이후에 다른 게임 컴포넌트들도 나열해야함
	switch (component->GetComponentType())
	{
		case ComponentType::Camera:
		{
			m_cameras.push_back(static_cast<Camera*>(component));
			break;
		}
		case ComponentType::DirectionalLight:
		case ComponentType::PointLight:
		case ComponentType::SpotLight:
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
		case ComponentType::AudioSource:
		{
			m_audioSources.push_back(static_cast<AudioSource*>(component));
			break;
		}
		case ComponentType::AudioListener:
		{
			m_audioListeners.push_back(static_cast<AudioListener*>(component));
			break;
		}
		case ComponentType::StaticMeshRenderer:
		{
			auto mr = static_cast<StaticMeshRenderer*>(component);
			if (mr->GetRenderStage() == RenderStage::Forward) return;
			m_staticMeshRenderers.push_back(mr);
			break;
		}
		case ComponentType::SkinnedMeshRenderer:
		{
			auto mr = static_cast<SkinnedMeshRenderer*>(component);
			if (mr->GetRenderStage() == RenderStage::Forward) return;
			m_skinnedMeshRenderers.push_back(mr);
			break;
		}
		case ComponentType::InstancedMeshRenderer:
		{
			auto mr = static_cast<InstancedMeshRenderer*>(component);
			if (mr->GetRenderStage() == RenderStage::Forward) return;
			m_instancedMeshRenderers.push_back(mr);
			break;
		}
		case ComponentType::MeshOutline:
		{
			m_outlines.push_back(static_cast<MeshOutline*>(component));
			break;
		}
		case ComponentType::UICanvas:
		{
			m_uiCanvases.push_back(static_cast<UICanvas*>(component));
			break;
		}
	}
}

GeneralRenderPass* Scene::GetCustomRenderPass(const std::string& name)
{
	auto it = m_customPasses.find(name);
	if (it != m_customPasses.end()) return it->second.get();
	return nullptr;
}

void Scene::AddCustomRenderPass(const std::string& name, GeneralRenderPassUPtr pass)
{
	if (m_customPasses.find(name) != m_customPasses.end())
		LOG_WARN("Custom random pass '{}' already exists. Overwriting.", name);

	m_customPasses[name] = std::move(pass);
}

void Scene::SetSkyLight(SkyLightUPtr skyLight)
{
	m_sky = std::move(skyLight);
}

/*=====================================//
//   default scene lifecycle methods   //
//=====================================*/
bool Scene::Init()
{
	if (!LoadNessesaryResources())
	{
		LOG_ERROR("failed to load resources.");
		return false;
	}

	if (!CreateNessesaryRenderPasses())
	{
		LOG_ERROR("failed to create render passes.");
		return false;
	}

	if (!CreateSceneContext())
	{
		LOG_ERROR("failed to create scene context.");
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
	// [Phase 1] Behaviors (Input)
	UpdateBehaviours();

	// [Phase 2] Systems (Output)
	UpdateSceneSystems();

	// [Phase 3] Cleanup (Destroy)
	FlushDestroyQueue();
}

/*================================//
//   scene update cycle methods   //
//================================*/
void Scene::UpdateBehaviours()
{
	// 1. 스크립트 Update
	for (auto* script : m_scripts) script->Update();

	// 2. 애니메이터 Update
	for (auto* animator : m_animators) animator->Update();
}

void Scene::UpdateSceneSystems()
{
	// 오디오
	for (auto* source : m_audioSources) source->Update();
	for (auto* listener : m_audioListeners) listener->Update();
}

void Scene::FlushDestroyQueue()
{
	// TODO : 씬에 Destroy를 호출한 오브젝트를 정리
}

