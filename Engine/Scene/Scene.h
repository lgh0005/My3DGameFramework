#pragma once
#include "Graphics/Uniform.h"
#include "Graphics/RenderContext.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(SceneRegistry)
CLASS_PTR(GameObjectManager)
CLASS_PTR(GameObject)
CLASS_PTR(GeneralRenderPass)
CLASS_PTR(SkyLight)
#pragma endregion

CLASS_PTR(Scene)
class Scene
{
public:
	virtual ~Scene();
	void OnScreenResize(int32 width, int32 height);

	// 1. 초기화 및 생명주기
	bool Init();		 // 리소스 로드 (엔진 레벨)
	void Awake();        // 1차 초기화 (객체 배치)
	void Start();        // 2차 초기화 (객체 연결)
	void Update();       // 메인 루프 (생성 -> 로직 -> 삭제)
	void LateUpdate();   // 후처리 루프 (카메라 등)

	// 2. 오브젝트 관리 인터페이스 (Manager에게 위임)
	void AddGameObject(GameObjectUPtr gameObject);

	// 3. 컴포넌트, 렌더 패스, 스카이 박스 접근 (Registry에게 위임)
	SceneRegistry* GetRegistry() { return m_registry.get(); }
	const SceneRegistry* GetRegistry() const { return m_registry.get(); }

	void AddRenderPass(const std::string& name, GeneralRenderPassUPtr renderPass);
	GeneralRenderPass* GetRenderPass(const std::string& name);
	void SetSkyLight(SkyLightUPtr skyLight);

protected:
	Scene();

	// 씬의 컨텍스트를 작성하는 가상 함수들
	virtual bool LoadSceneResources()	    = 0;
	virtual bool CreateCustomRenderPasses() = 0;
	virtual bool SetupSceneEnvironment()    = 0;
	virtual bool OnPlaceActors()			= 0;
	virtual bool OnBeginPlay()				= 0;

protected:
	// 내부 로직 분리 (Update를 깔끔하게 유지하기 위함)
	void ProcessPendingAdds();  // 생성 대기열 처리
	void ProcessPendingKills(); // 삭제 대기열 처리

	SceneRegistryUPtr     m_registry;      // 데이터 저장소 (Component Ptrs)
	GameObjectManagerUPtr m_objectManager; // 수명 관리자 (GameObject Ptrs)

	bool m_isAwake		{ false };
	bool m_isStarted	{ false };
};