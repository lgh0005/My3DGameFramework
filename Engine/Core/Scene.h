#pragma once
#include "Graphics/Uniform.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Component)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(Camera)
CLASS_PTR(Animator)
CLASS_PTR(Script)
CLASS_PTR(AudioSource)
CLASS_PTR(AudioListener)
CLASS_PTR(GeneralRenderPass)
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

	void SetSkyboxTexture(CubeTexturePtr texture) { m_skyboxTexture = texture; }
	CubeTexture* GetSkyboxTexture() const { return m_skyboxTexture.get(); }

	GeneralRenderPass* GetCustomRenderPass(const std::string& name);

/*================================//
//   interface to renderContext   //
//================================*/
public:
	void AddCustomRenderPass(const std::string& name, GeneralRenderPassUPtr pass);
	auto& GetCustomRenderPasses() { return m_customPasses; }

	auto& GetStaticMeshRenderers() { return m_staticMeshRenderers; }
	auto& GetSkinnedMeshRenderers() { return m_skinnedMeshRenderers; }

protected:
	Scene();

	// 씬의 컨텍스트를 작성하는 가상 함수들
	virtual bool LoadNessesaryResources()	   = 0;
	virtual bool CreateNessesaryRenderPasses() = 0;
	virtual bool CreateSceneContext()          = 0;
	bool Init();

	// 게임 오브젝트 소유권
	std::vector<GameObjectUPtr> m_gameObjects;

	// 삭제 대기열
	// TODO : GameObject 파괴를 위한 FlushDestroyQueue 구현 진행 필요
	void FlushDestroyQueue();
	std::vector<GameObjectUPtr> m_destroyQueue;

	// 주요 참조 (캐싱)
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

	// 하늘 텍스쳐
	CubeTexturePtr				m_skyboxTexture;

	// 커스텀 포워드 렌더 패스
	std::unordered_map<std::string, GeneralRenderPassUPtr> m_customPasses;
};