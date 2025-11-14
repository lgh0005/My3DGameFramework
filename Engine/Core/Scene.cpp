#include "EnginePch.h"
#include "Scene.h"

#include "Core/GameObject.h"
#include "Components/Bases/Light.h"
#include "Components/MeshRenderer.h"
#include "Components/Camera.h"
#include "Components/Animator.h"

void Scene::AddGameObject(GameObjectUPtr gameObject)
{
	auto* go = gameObject.get();

	for (const auto& comp : go->GetAllComponents()) 
	{
		// swtich-case 문으로 컴포넌트 분리
		switch (comp->GetType())
		{
			case ComponentType::Camera:
			{
				auto* camera = static_cast<Camera*>(comp.get());
				m_cameras.push_back(camera);

				// 씬에 추가된 첫 번째 카메라를 활성 카메라로 자동 설정
				if (!m_activeCamera)
				{
					m_activeCamera = camera;
				}
				break;
			}
			case ComponentType::MeshRenderer:
			{
				m_meshes.push_back(static_cast<MeshRenderer*>(comp.get()));
				break;
			}
			case ComponentType::Light:
			{
				m_lights.push_back(static_cast<Light*>(comp.get()));
				break;
			}
			case ComponentType::Animator:
			{
				m_animators.push_back(static_cast<Animator*>(comp.get()));
				break;
			}
			default: break;
		}
	}

	m_gameObjects.push_back(std::move(gameObject));
}

void Scene::Update()
{
	// TODO : 나중에 추가할 Destroy/SetActive 로직을 고려

	// TODO
	// 1. 캐시된 m_animators 목록을 순회하며 애니메이션 업데이트
	for (auto* animator : m_animators)
	{
		// (나중에 추가) if (animator->GetOwner()->IsActiveInHierarchy())

		// 기존 Context::Render()에 있던 애니메이션 업데이트 로직을
		// Scene::Update()로 이동
		animator->UpdateAnimation();
	}
	// 
	// 2. (미래) 스크립트 컴포넌트 업데이트
	// for (auto* script : m_scripts)
	// {
	//     script->Update(deltaTime);
	// }

	// 3. (미래) 파괴가 예약된 오브젝트들 일괄 삭제
	// FlushDestroyQueue();
}
