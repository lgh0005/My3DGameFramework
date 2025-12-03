#include "EnginePch.h"
#include "Scene.h"

#include "Core/GameObject.h"
#include "Graphics/Mesh.h"
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
#include "Components/AudioSource.h"
#include "Components/AudioListener.h"

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
		case ComponentType::MeshRenderer:
		{
			auto meshRenderer = static_cast<MeshRenderer*>(component);
			MeshType meshType = meshRenderer->GetMesh()->GetMeshType();
			RenderStage stage = meshRenderer->GetRenderStage();

			if (stage == RenderStage::Forward) return;

			switch (meshType)
			{
				case MeshType::Static:
				{
					m_staticMeshRenderers.push_back(meshRenderer);
					break;
				}

				case MeshType::Skinned:
				{
					m_skinnedMeshRenderers.push_back(meshRenderer);
					break;
				}
				case MeshType::Instanced:
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
	}
}

void Scene::OnScreenResize(int32 width, int32 height)
{
	// TODO : 이후에는 다중 카메라에 대해서 모든 카메라가 리사이징 되어야함
	auto* camera = GetMainCamera();
	if (camera) camera->SetAspectRatio((float)width / (float)height);
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

	// 전체적으로 컴포넌트가 계층적 위치에 따라 적용되도록 만들 필요가 있음
	
	// 2. 스크립트 컴포넌트 업데이트
	for (auto* script : m_scripts)
	{
	     script->Update();
	}

	// 3. 오디오 소스 위치 갱신
	for (auto* audioSource : m_audioSources)
	{
		audioSource->Update();
	}

	// 3. 오디오 리스너 위치 갱신
	// TODO : 리스너는 단 하나만 있도록 유도 필요
	for (auto* audioListener : m_audioListeners)
	{
		audioListener->Update();
	}

	// 3. 파괴가 예약된 오브젝트들 일괄 삭제
	FlushDestroyQueue();
}

void Scene::FlushDestroyQueue()
{
	// TODO : 씬에 Destroy를 호출한 오브젝트를 정리
}

RenderPass* Scene::GetCustomRenderPass(const std::string& name)
{
	auto it = m_customPasses.find(name);
	if (it != m_customPasses.end()) return it->second.get();
	return nullptr;
}

void Scene::AddCustomRenderPass(const std::string& name, RenderPassUPtr pass)
{
	if (m_customPasses.find(name) != m_customPasses.end())
		SPDLOG_WARN("Custom random pass '{}' already exists. Overwriting.", name);

	m_customPasses[name] = std::move(pass);
}