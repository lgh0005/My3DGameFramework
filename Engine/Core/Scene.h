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

	GeneralRenderPass* GetCustomRenderPass(const std::string& name);

	// TODO : 이후에 SceneEnvironment 도입 가능
#pragma region SCENE_ENVIRONMENT_METHODS
	// 지금은 SRP, PBR과 관련없이 일단 하늘과 관련된 텍스쳐의 getter/setter를 여기에
	// 적어두겠음
	void SetSkyboxTexture(CubeTexturePtr texture) { m_skyboxTexture = texture; }
	CubeTexture* GetSkyboxTexture() const { return m_skyboxTexture.get(); }

	void SetIrradianceTexture(CubeTexturePtr texture) { m_irradianceTexture = texture; }
	CubeTexture* GetIrradianceTexture() const { return m_irradianceTexture.get(); }

	void SetPrefilteredTexture(CubeTexturePtr texture) { m_prefiteredTexture = texture; }
	CubeTexture* GetPrefilteredTexture() const { return m_prefiteredTexture.get(); }

	void SetBRDFLookUpTexture(TexturePtr texture) { m_brdfLookUpTexture = texture; }
	Texture* GetBRDFLookUpTexture() const { return m_brdfLookUpTexture.get(); }
#pragma endregion

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
	// TODO : 이후에 SRP와 PBR 모두를 고려하여 여기에 늘어나는
	// 포인터 멤버들을 하나의 SceneEnvironment과 같은 구조체로
	// 묶어서 관리할 수도 있음.
	// 각 맵들에 대한 setter와 getter를 달아둔 클래스로 관리하면 편리할 수 있음.
	CubeTexturePtr				m_skyboxTexture;
	CubeTexturePtr			    m_irradianceTexture;
	CubeTexturePtr				m_prefiteredTexture;
	TexturePtr					m_brdfLookUpTexture;

	// 커스텀 포워드 렌더 패스
	std::unordered_map<std::string, GeneralRenderPassUPtr> m_customPasses;
};