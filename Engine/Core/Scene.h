#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(Camera)
CLASS_PTR(Animator)
#pragma endregion

CLASS_PTR(Scene)
class Scene
{
public:
	virtual ~Scene();

	void Update();
	void AddGameObject(GameObjectUPtr gameObject);
	void AddRenderPass(const std::string& name, RenderPassUPtr renderPass);

	auto& GetRenderPasses() const { return m_renderPasses; }
	RenderPass* GetRenderPass(const std::string& name) { return m_renderPasses[name].get(); }
	void SetShadowPass(ShadowPassUPtr pass) { m_shadowPass = std::move(pass); }
	ShadowPass* GetShadowPass() const { return m_shadowPass.get(); }
	void SetSkyboxPass(SkyboxPassUPtr pass) { m_skyboxPass = std::move(pass); }
	SkyboxPass* GetSkyboxPass() const { return m_skyboxPass.get(); }
	void SetPostProcessPass(PostProcessPassUPtr pass) { m_postProcessPass = std::move(pass); }
	PostProcessPass* GetPostProcessPass() const { return m_postProcessPass.get(); }

	const std::vector<Light*>& GetLights() const { return m_lights; }
	const std::vector<Camera*>& GetAllCameras() const { return m_cameras; }
	Camera* GetMainCamera() const { return m_mainCamera; }
	void SetMainCamera(Camera* camera) { m_mainCamera = camera; }
	Light* GetMainLight() const { return m_mainLight; }
	void SetMainLight(Light* light) { m_mainLight = light; }

	// TODO: 
	// - GameObject 파괴를 위한 FlushDestroyQueue 필요
	// - GameObject를 이름 등으로 찾는 FindGameObject 필요

protected:
	Scene() = default;
	
	// 씬의 컨텍스트를 작성하는 가상 함수들
	virtual bool LoadNessesaryResources()	   = 0;
	virtual bool CreateNessesaryRenderPasses() = 0;
	virtual bool CreateSceneContext()          = 0;
	bool Init();

	// 모든 게임 오브젝트를 소유
	std::vector<GameObjectUPtr> m_gameObjects;

	// TEMP : 씬의 디폴트 속성들
	Camera* m_mainCamera		{ nullptr };
	Light* m_mainLight			{ nullptr };

	// 컴포넌트 업데이트를 위한 참조 포인터
	std::vector<Light*>			m_lights;
	std::vector<Camera*>		m_cameras;
	std::vector<Animator*>		m_animators;

	// 렌더링 멤버들 : 일반 렌더링과 고정적으로 필요한 렌더링
	std::unordered_map<std::string, RenderPassUPtr> m_renderPasses;
	ShadowPassUPtr				m_shadowPass		{ nullptr };
	SkyboxPassUPtr				m_skyboxPass		{ nullptr };
	PostProcessPassUPtr			m_postProcessPass	{ nullptr };
};