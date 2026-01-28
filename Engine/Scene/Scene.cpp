#include "EnginePch.h"
#include "Scene.h"

#include "Object/GameObject.h"
#include "Scene/ComponentRegistry.h"
#include "Scene/GameObjectRegistry.h"
#include "Scene/SceneRegistry.h"
#include "Graphics/RenderPass.h"
#include "Components/SkyLight.h"
#include "Components/Camera.h"
#include "Components/Transform.h"

DECLARE_DEFAULTS_IMPL(Scene)

bool Scene::Init()
{
	// 0. 이미 초기화 된 것이라면 false
	if (!SCENE.IsUninitialized()) return false;

	// 1. 씬 상태를 Loading으로 전이
	m_sceneRegistry = SceneRegistry::Create();
	if (!m_sceneRegistry) return false;
	SCENE.SetSceneState(SceneState::Loading);

	// 2. 엔진 레벨 리소스 로드
	if (!LoadSceneResources()) return false;

	// 3. 렌더 패스 설정
	if (!CreateCustomRenderPasses()) return false;

	// 4. Skybox과 같은 환경 설정
	if (!SetupSceneEnvironment()) return false;

	// 5. Actor들을 배치
	if (!OnPlaceActors()) return false;

	return true;
}

void Scene::OnScreenResize(int32 width, int32 height)
{
	// [변경 1] GetCameras() 삭제됨 -> GetComponents<Camera>() 사용
	const auto& cameraComponents = GetComponentRegistry()->GetComponents<Camera>();

	// [변경 2] Component* 벡터이므로 캐스팅 필요
	for (Component* comp : cameraComponents)
	{
		auto* cam = static_cast<Camera*>(comp);
		cam->SetViewportSize((float)width, (float)height);
	}
}

/*======================================//
//   default scene life-cycle methods   //
//======================================*/
void Scene::Awake()
{
	if (SCENE.IsSceneAwake()) return;

	// 2. 상태를 Awake로 전이
	SCENE.SetSceneState(SceneState::Awake);

	// 3. 대기열에 있는 객체들을 즉시 깨움
	ProcessPendingAdds();
}

void Scene::Start()
{
	if (SCENE.IsSceneRunning()) return;

	// 1. 사용자 정의 Start 실행
	OnBeginPlay();

	// 2. 상태를 Start로 전이
	SCENE.SetSceneState(SceneState::Running);

	// 3. 혹시 OnStart 중에 추가된 객체가 있다면 처리
	ProcessPendingAdds();

	// 4. 기존에 Awake된 객체들 중, 아직 Start 안 된 녀석들을 Start 시킴
	const auto& gameObjects = GetGameObjectRegistry()->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (go->IsActive()) go->Start();
	}
}

void Scene::FixedUpdate()
{
	if (!SCENE.IsSceneRunning()) return;

	// [Phase 2] 물리 업데이트 (FixedUpdate)
	for (const auto& go : GetGameObjectRegistry()->GetGameObjects())
	{
		if (go->IsActive()) go->FixedUpdate();
	}
}

void Scene::Update()
{
	if (!SCENE.IsSceneRunning()) return;

	// [중요] 매 프레임 초입에 신규 생성 객체 처리 (Catch-up)
	ProcessPendingAdds();

	// [Phase 3] 게임 로직 업데이트 (Update)
	for (const auto& go : GetGameObjectRegistry()->GetGameObjects())
	{
		if (go->IsActive()) go->Update();
	}

	// [Phase 4] 삭제 대기열 처리 (보통 Update 끝난 후)
	ProcessPendingKills();
}

void Scene::LateUpdate()
{
	// [Phase 5] 후처리 업데이트
	if (!SCENE.IsSceneRunning()) return;

	// [Phase 5] 후처리 업데이트 (LateUpdate)
	// 카메라 추적 등 로직 이후에 처리되어야 하는 작업들
	const auto& gameObjects = GetGameObjectRegistry()->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (go->IsActive()) go->LateUpdate();
	}
}

/*================================//
//   object utilities for scene   //
//================================*/
void Scene::AddGameObject(GameObjectUPtr gameObject)
{
	GetGameObjectRegistry()->AddGameObject(std::move(gameObject));
}

void Scene::Destroy(GameObject* obj)
{
	GetGameObjectRegistry()->DestroyGameObject(obj);
}

GameObject* Scene::FindObjectByID(InstanceID id)
{
	if (!m_sceneRegistry) return nullptr;
	return m_sceneRegistry->GetGameObjectRegistry()->GetGameObjectByID(id);
}

GameObject* Scene::FindObjectByName(const std::string& name)
{
	if (!m_sceneRegistry) return nullptr;
	return m_sceneRegistry->GetGameObjectRegistry()->FindGameObjectByName(name);
}

/*==================================//
//   object management for scene    //
//==================================*/
void Scene::ProcessPendingAdds()
{
	// 1. 매니저에게 "새 친구들 명단" 요청
	const auto& pendingAdds = GetGameObjectRegistry()->GetPendingCreateQueue();
	if (pendingAdds.empty()) return;

	for (const auto& go : pendingAdds)
	{
		// 1-1. [Registry] 컴포넌트 등록 (렌더링, 시스템 등을 위해 필수)
		for (const auto& comp : go->GetComponents())
		{
			GetComponentRegistry()->RegisterComponent(comp.get());
		}

		// 1-2. [Lifecycle] 생명주기 따라잡기 (Catch-up)
		// 씬이 이미 Awake 상태라면 -> 객체도 Awake
		if (SCENE.IsSceneAwake()) go->Awake();

		// 씬이 이미 Start 상태라면 -> 객체도 Start (활성화된 경우만)
		if (SCENE.IsSceneRunning() && go->IsActive()) go->Start();
	}

	// 2. [Manager] 승인 (이제 진짜 리스트로 이동)
	GetGameObjectRegistry()->FlushCreateQueue();
}

void Scene::ProcessPendingKills()
{
	// 1. 매니저에게 "죽을 친구들 명단" 요청
	const auto& pendingKills = GetGameObjectRegistry()->GetPendingDestroyQueue();
	if (pendingKills.empty()) return;

	for (GameObject* deadObj : pendingKills)
	{
		// 1-1. [Lifecycle] 최후의 유언 (OnDestroy)
		deadObj->OnDestroy();

		// 1-2. [Registry] 컴포넌트 말소 (Dangling Pointer 방지)
		for (const auto& comp : deadObj->GetComponents())
			GetComponentRegistry()->UnregisterComponent(comp.get());

		// 1-3. 부모를 nullptr로 설정하면, Transform 내부에서 알아서
		// 기존 부모의 자식 리스트에서 나를 빼는(RemoveChild) 작업이 수행
		Transform& myTransform = deadObj->GetTransform();
		Transform* parentTransform = myTransform.GetParent();
		if (parentTransform)
		{
			GameObject* parentGO = parentTransform->GetOwner();

			// 부모가 존재하고, 부모가 "안 죽는 상태"일 때만 연결을 끊음.
			// 부모가 죽고 있다면(IsDead), 굳이 RemoveChild를 호출해서 오버헤드를 줄 필요 없음.
			// 쉽게 말해, 자식을 꼭 방문해서 끊는 것이 아니라 부모가 끊기면 그 아래는
			// 쳐다 보지도 말고 그냥 전부 끊어버리는 구조.
			if (parentGO && !parentGO->IsDead()) myTransform.SetParent(nullptr);
		}
	}

	// 2. [Manager] 승인 (메모리 해제)
	GetGameObjectRegistry()->FlushDestroyQueue();
}

/*================================================//
//   scene property getters from SceneRegistry    //
//================================================*/
ComponentRegistry* Scene::GetComponentRegistry()
{
	return m_sceneRegistry->GetComponentRegistry();
}

GameObjectRegistry* Scene::GetGameObjectRegistry()
{
	return m_sceneRegistry->GetGameObjectRegistry();
}

GeneralRenderPass* Scene::GetRenderPass(const std::string& name)
{
	return GetComponentRegistry()->GetCustomRenderPass(name);
}

void Scene::AddRenderPass(const std::string& name, GeneralRenderPassUPtr renderPass)
{
	GetComponentRegistry()->AddCustomRenderPass(name, std::move(renderPass));
}

void Scene::SetSkyLight(SkyLightUPtr skyLight)
{
	GetComponentRegistry()->SetSkyLight(std::move(skyLight));
}
