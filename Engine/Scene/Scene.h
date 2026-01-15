#pragma once
#include "Graphics/Uniform.h"
#include "Graphics/RenderContext.h"
#include "Scene/SceneUtils.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(SkyLight)
CLASS_PTR(GameObject)
CLASS_PTR(GeneralRenderPass)
CLASS_PTR(ComponentRegistry)
CLASS_PTR(GameObjectRegistry)
CLASS_PTR(SceneRegistry)
#pragma endregion

CLASS_PTR(Scene)
class Scene
{
	friend class SceneUtils;

public:
	virtual ~Scene();
	bool Init();		 // 리소스 로드 및 컨텍스트 설정(엔진 레벨)
	void OnScreenResize(int32 width, int32 height);
	void SetSceneState(SceneState state) { m_state = state; }
	SceneState GetSceneState() const	 { return m_state; }

/*======================================//
//   default scene life-cycle methods   //
//======================================*/
public:
	// 1. 초기화 및 생명주기
	void Awake();        // 1차 초기화 (객체 배치)
	void Start();        // 2차 초기화 (객체 연결)
	void FixedUpdate();  // 물리 업데이트 시작
	void Update();       // 메인 루프 (생성 -> 로직 -> 삭제)
	void LateUpdate();   // 후처리 루프 (카메라 등)

/*================================//
//   object utilities for scene   //
//================================*/
public:
	// 2. 오브젝트 관리 인터페이스 (Manager에게 위임)
	void AddGameObject(GameObjectUPtr gameObject);
	void Destroy(GameObject* obj);

/*==================================//
//   object management for scene    //
//==================================*/
public:
	// 4. 파괴 예약된 오브젝트를 삭제하기
	void ProcessPendingAdds();  // 생성 대기열 처리
	void ProcessPendingKills(); // 삭제 대기열 처리

/*=================================//
//   getters from SceneRegistry    //
//=================================*/
public:
	// 3. SceneRegistry에 있는 컴포넌트, 렌더 패스, 스카이 박스 접근
	ComponentRegistry* GetRegistry() { return m_registry.get(); }
	const ComponentRegistry* GetRegistry() const { return m_registry.get(); }
	void SetSkyLight(SkyLightUPtr skyLight);
	void AddRenderPass(const std::string& name, GeneralRenderPassUPtr renderPass);
	GeneralRenderPass* GetRenderPass(const std::string& name);

/*=======================================//
//   abstract methods for user context   //
//=======================================*/
protected:
	Scene();

	// 씬의 컨텍스트를 작성하는 가상 함수들
	virtual bool LoadSceneResources()	    = 0;
	virtual bool CreateCustomRenderPasses() = 0;
	virtual bool SetupSceneEnvironment()    = 0;
	virtual bool OnPlaceActors()			= 0;
	virtual bool OnBeginPlay()				= 0;

/*=======================================//
//   abstract methods for user context   //
//=======================================*/
protected:
	ComponentRegistryUPtr     m_registry;			// 데이터 저장소 (Component Ptrs)
	GameObjectRegistryUPtr    m_objectManager;		// 수명 관리자 (GameObject Ptrs)

	SceneRegistryUPtr		  m_sceneRegistry;
	SceneState			      m_state{ SceneState::Uninitialized };
};