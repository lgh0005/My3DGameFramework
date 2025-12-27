#include "EnginePch.h"
#include "Scene.h"

#include "Core/GameObject.h"
#include "Core/RenderPass.h"
#include "Resources/Mesh.h"
#include "Graphics/SkyLight.h"
#include "Components/Component.h"
#include "Components/Light.h"
#include "Components/PointLight.h"
#include "Components/DirectionalLight.h"
#include "Components/MeshRenderer.h"
#include "Components/MeshOutline.h"
#include "Components/Camera.h"
#include "Components/Animator.h"
#include "Components/Script.h"
#include "Components/Transform.h"
#include "Components/AudioSource.h"
#include "Components/AudioListener.h"

Scene::Scene() = default;
Scene::~Scene() = default;

/*===================================================================//
//   object, component and custom render passes management methods   //
//===================================================================*/
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
		// TODO : 중첩 switch-case문은 좋은 방식은 아니므로 
		// 가독성을 위해 따로 함수로 뺄 필요 있음
		// TODO : 이후에 MeshRenderer를 상위 클래스로 StaticMeshRenderer, SkinnedMeshRenderer 등이
		// 생겨날 수도 있음. 그럴 경우에는 위의 Light처럼 처리를 해야 할 수도 있다.
		// InstancedMesh에 대한 분화는 좀 더 생각을 해봐야 함.
		case ComponentType::MeshRenderer:
		{
			auto meshRenderer = static_cast<MeshRenderer*>(component);
			ResourceType meshType = meshRenderer->GetMesh()->GetResourceType();
			RenderStage stage = meshRenderer->GetRenderStage();

			if (stage == RenderStage::Forward) return;

			switch (meshType)
			{
				case ResourceType::StaticMesh:
				{
					m_staticMeshRenderers.push_back(meshRenderer);
					break;
				}

				case ResourceType::SkinnedMesh:
				{
					m_skinnedMeshRenderers.push_back(meshRenderer);
					break;
				}
				case ResourceType::InstancedMesh:
				{
					// TODO : 메쉬의 어떤 속성을 다르게 하여 인스턴싱을
					// 할 지는 모르기 때문에 이는 따로 처리할 필요가 있음
					// 현재는 커스텀 포워드 렌더링 패스를 제작해서 넣는 것으로 함.
					// 이쪽은 예를 들어, 조명의 영향을 받는 무수한 풀밭을 그리고 
					// 싶을 때 활용할 수 있음.
					break;
				}
			}
			
			break;
		}
		case ComponentType::MeshOutline:
		{
			m_outlines.push_back(static_cast<MeshOutline*>(component));
			break;
		}
	}
}

void Scene::OnScreenResize(int32 width, int32 height)
{
	// TODO : 이후에는 다중 카메라에 대해서 모든 카메라가 리사이징 되어야함
	auto* camera = GetMainCamera();
	if (camera) camera->SetAspectRatio((float)width / (float)height);
}

void Scene::SetSkyLight(SkyLightUPtr skyLight)
{
	m_sky = std::move(skyLight);
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
		SPDLOG_WARN("Custom random pass '{}' already exists. Overwriting.", name);

	m_customPasses[name] = std::move(pass);
}

/*=====================================//
//   default scene lifecycle methods   //
//=====================================*/
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
	// [Phase 1] Behaviors (Input)
	UpdateBehaviours();

	// [Phase 2] Transforms (Calculation)
	UpdateTransforms();

	// [Phase 3] Systems (Output)
	UpdateSceneSystems();

	// [Phase 4] Cleanup (Destroy)
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

void Scene::UpdateTransforms()
{
	// 1. 루트 오브젝트만 찾아서 계층 순회 시작
	for (auto& go : m_gameObjects)
	{
		if (go->GetTransform().GetParent() == nullptr)
			UpdateTransformRecursive(go.get());
	}
}

void Scene::UpdateTransformRecursive(GameObject* go)
{
	if (!go) return;

	// [SetActive 로직] 
	// Unity/Unreal 방식: 부모가 비활성되면 자식도 로직상 비활성 취급
	// 따라서 여기서 IsActive 체크를 해서, 꺼져있으면 자식들 순회도 안 하고 리턴
	// if (!go->IsActive()) return;

	// 1. 내 월드 행렬 갱신
	go->GetTransform().Update();

	// 2. 자식들 재귀 호출
	for (Transform* childTransform : go->GetTransform().GetChildren())
	{
		GameObject* childGO = childTransform->GetOwner();
		if (childGO) UpdateTransformRecursive(childGO);
	}
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

