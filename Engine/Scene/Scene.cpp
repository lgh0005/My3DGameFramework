#include "EnginePch.h"
#include "Scene.h"

#include "GameObject/GameObject.h"
#include "Scene/SceneRegistry.h"
#include "Scene/GameObjectManager.h"
#include "Graphics/RenderPass.h"
#include "Graphics/SkyLight.h"
#include "Components/Camera.h"
#include "Components/Transform.h"

Scene::Scene()
{
	m_registry = SceneRegistry::Create();
	m_objectManager = GameObjectManager::Create();
}
Scene::~Scene() = default;

/*===================================================//
//    Scene Lifecycle Flow (System Initialization)   //
//===================================================*/
bool Scene::Init()
{
	// 1. 엔진 레벨 리소스 로드
	if (!LoadSceneResources()) return false;

	// 2. 렌더 패스 설정
	if (!CreateCustomRenderPasses()) return false;

	// 3. 씬 컨텍스트 설정 (환경 변수 등)
	if (!SetupSceneEnvironment()) return false;

	return true;
}

void Scene::Awake()
{
	if (m_isAwake) return;

	// 1. 사용자 정의 Awake 실행 (여기서 GameObject들이 Add됨)
	OnPlaceActors();

	// 2. 대기열에 있는 객체들을 즉시 깨움
	ProcessPendingAdds();

	m_isAwake = true;
}

void Scene::Start()
{
	if (m_isStarted) return;

	// 1. 사용자 정의 Start 실행
	OnBeginPlay();

	// 2. 혹시 OnStart 중에 추가된 객체가 있다면 처리
	ProcessPendingAdds();

	// 3. 기존에 Awake된 객체들 중, 아직 Start 안 된 녀석들을 Start 시킴
	const auto& gameObjects = m_objectManager->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (go->IsActiveInHierarchy()) go->Start();
	}

	m_isStarted = true;
}

void Scene::FixedUpdate()
{
	// [Phase 2] 물리 업데이트 (FixedUpdate)
	for (const auto& go : m_objectManager->GetGameObjects())
	{
		if (go->IsActiveInHierarchy()) go->FixedUpdate();
	}
}

void Scene::Update()
{
	// [Phase 3] 게임 로직 업데이트 (Update)
	for (const auto& go : m_objectManager->GetGameObjects())
	{
		if (go->IsActiveInHierarchy()) go->Update();
	}
}

void Scene::LateUpdate()
{
	// [Phase 5] 후처리 업데이트 (LateUpdate)
	// 카메라 추적 등 로직 이후에 처리되어야 하는 작업들
	const auto& gameObjects = m_objectManager->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (go->IsActiveInHierarchy()) go->LateUpdate();
	}
}

void Scene::ProcessPendingAdds()
{
	// 1. 매니저에게 "새 친구들 명단" 요청
	const auto& pendingAdds = m_objectManager->GetPendingCreateQueue();
	if (pendingAdds.empty()) return;

	for (const auto& go : pendingAdds)
	{
		// 1-1. [Registry] 컴포넌트 등록 (렌더링, 시스템 등을 위해 필수)
		for (const auto& comp : go->GetAllComponents())
		{
			m_registry->RegisterComponent(comp.get());
		}

		// 1-2. [Lifecycle] 생명주기 따라잡기 (Catch-up)
		// 씬이 이미 Awake 상태라면 -> 객체도 Awake
		if (m_isAwake) go->Awake();

		// 씬이 이미 Start 상태라면 -> 객체도 Start (활성화된 경우만)
		if (m_isStarted && go->IsActiveInHierarchy()) go->Start();
	}

	// 2. [Manager] 승인 (이제 진짜 리스트로 이동)
	m_objectManager->FlushCreateQueue();
}

void Scene::ProcessPendingKills()
{
	// 1. 매니저에게 "죽을 친구들 명단" 요청
	const auto& pendingKills = m_objectManager->GetPendingDestroyQueue();
	if (pendingKills.empty()) return;

	for (GameObject* deadObj : pendingKills)
	{
		// 1-1. [Lifecycle] 최후의 유언 (OnDestroy)
		deadObj->OnDestroy();

		// 1-2. [Registry] 컴포넌트 말소 (Dangling Pointer 방지)
		for (const auto& comp : deadObj->GetAllComponents())
			m_registry->UnregisterComponent(comp.get());

		// 1-3. 부모를 nullptr로 설정하면, Transform 내부에서 알아서
		// 기존 부모의 자식 리스트에서 나를 빼는(RemoveChild) 작업이 수행
		deadObj->GetTransform().SetParent(nullptr);
	}

	// 2. [Manager] 승인 (메모리 해제)
	m_objectManager->FlushDestroyQueue();
}

/*============================//
//    scene context methods   //
//============================*/
void Scene::AddGameObject(GameObjectUPtr gameObject)
{
	m_objectManager->AddGameObject(std::move(gameObject));
}

GameObject* Scene::FindGameObject(const std::string& name)
{
	return m_objectManager->FindGameObject(name);
}

void Scene::Destroy(GameObject* obj)
{
	m_objectManager->DestroyGameObject(obj);
}

void Scene::OnScreenResize(int32 width, int32 height)
{
	// Registry에 있는 모든 카메라의 비율을 갱신
	// (이제 MainCamera 변수가 없어도 Registry 덕분에 모든 카메라 제어 가능)
	const auto& cameras = m_registry->GetCameras();
	for (Camera* cam : cameras)
	{
		cam->SetViewportSize((float)width, (float)height);
	}
}

void Scene::AddRenderPass(const std::string& name, GeneralRenderPassUPtr renderPass)
{
	m_registry->AddCustomRenderPass(name, std::move(renderPass));
}

GeneralRenderPass* Scene::GetRenderPass(const std::string& name)
{
	return m_registry->GetCustomRenderPass(name);
}

void Scene::SetSkyLight(SkyLightUPtr skyLight)
{
	m_registry->SetSkyLight(std::move(skyLight));
}

