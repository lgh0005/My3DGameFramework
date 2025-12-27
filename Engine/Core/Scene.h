#pragma once
#include "Graphics/Uniform.h"
#include "RenderContext.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Component)
CLASS_PTR(MeshRenderer)
CLASS_PTR(MeshOutline)
CLASS_PTR(Light)
CLASS_PTR(Camera)
CLASS_PTR(Animator)
CLASS_PTR(Script)
CLASS_PTR(AudioSource)
CLASS_PTR(AudioListener)
CLASS_PTR(GeneralRenderPass)
CLASS_PTR(SkyLight)
#pragma endregion

CLASS_PTR(Scene)
class Scene
{
public:
	virtual ~Scene();
	void Start();
	void Update();

	void AddGameObject(GameObjectUPtr gameObject);
	void RegisterComponent(Component* component);
	void OnScreenResize(int32 width, int32 height);

	// TODO : 추가해야 할 메서드
	// 1. Find : 씬에서 특정 게임 오브젝트 찾는 메서드

/*=======================================//
//   scene property getter and setters   //
//=======================================*/
public:
	const std::vector<Light*>& GetLights() const { return m_lights; }
	const std::vector<Camera*>& GetAllCameras() const { return m_cameras; }
	Camera* GetMainCamera() const { return m_mainCamera; }
	void SetMainCamera(Camera* camera) { m_mainCamera = camera; }
	SkyLight* GetSkyLight() const { return m_sky.get(); }
	void SetSkyLight(SkyLightUPtr skyLight);
	GeneralRenderPass* GetCustomRenderPass(const std::string& name);

/*================================//
//   interface to renderContext   //
//================================*/
public:
	void AddCustomRenderPass(const std::string& name, GeneralRenderPassUPtr pass);
	auto& GetCustomRenderPasses() { return m_customPasses; }
	auto& GetStaticMeshRenderers() { return m_staticMeshRenderers; }
	auto& GetSkinnedMeshRenderers() { return m_skinnedMeshRenderers; }
	auto& GetMeshOutlines() { return m_outlines; }

/*================================//
//   scene update cycle methods   //
//================================*/
protected:
	// TODO : 이후 Destroy와 같은 것을 구현할 때,
	// 자신이 파괴될 지 말지를 큐에 넣는 메서드가 필요.
	// TODO : 메서드 이름 좀 더 고려 필요. 일단 임시로 이렇게 적겠음.
	void UpdateBehaviours();
	void UpdateTransforms();
	void UpdateTransformRecursive(GameObject* go);
	void UpdateSceneSystems();
	void FlushDestroyQueue();
	std::vector<GameObjectUPtr> m_destroyQueue;

protected:
	Scene();

	// 씬의 컨텍스트를 작성하는 가상 함수들
	virtual bool LoadNessesaryResources()	   = 0;
	virtual bool CreateNessesaryRenderPasses() = 0;
	virtual bool CreateSceneContext()          = 0;
	bool Init();

	// 게임 오브젝트 소유권
	std::vector<GameObjectUPtr> m_gameObjects;

	// 주요 참조 (캐싱)
	// TOOD : 이후에는 Main Camera를 딱히 이렇게 하나의 변수로 다믄 것이 아니라
	// m_cameras에서 렌더링할 카메라를 가져오는 방식으로 만들어야 한다.
	Camera* m_mainCamera		{ nullptr };

	// 컴포넌트 업데이트를 위한 참조 포인터 벡터
	std::vector<Light*>			m_lights;
	std::vector<Camera*>		m_cameras;
	std::vector<Animator*>		m_animators;
	std::vector<Script*>        m_scripts;
	std::vector<AudioSource*>   m_audioSources;
	std::vector<AudioListener*> m_audioListeners;
	std::vector<MeshRenderer*>  m_staticMeshRenderers;
	std::vector<MeshRenderer*>  m_skinnedMeshRenderers;
	std::vector<MeshOutline*>   m_outlines;

	// 하늘 텍스쳐
	SkyLightUPtr				m_sky;

	// 커스텀 포워드 렌더 패스
	std::unordered_map<std::string, GeneralRenderPassUPtr> m_customPasses;
};

// TODO : Scene 스크립팅 API 추가 고려 필요
// Utils보다는 다른 class를 추가하는 것을 고려해볼 필요가 있다.
// 어렵지 않게 Scene과 friend를 맺게 해서 멤버 접근을 열어두면 됨.
// 씬 스크립팅 API니까 friend 당위성은 충분함.
// 또는, Script 클래스는 Scene 클래스와 friend를 맺어도 됨.