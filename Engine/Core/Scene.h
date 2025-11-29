#pragma once
#include "Core/RenderPass.h"
#include "Graphics/Uniform.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Component)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(Camera)
CLASS_PTR(Animator)
CLASS_PTR(Script)
CLASS_PTR(Uniformbuffer)
#pragma endregion

CLASS_PTR(Scene)
class Scene
{
public:
	virtual ~Scene();

	void Start();
	void Update();

	void AddGameObject(GameObjectUPtr gameObject);
	void AddRenderPass(const std::string& name, RenderPassUPtr renderPass);
	void RegisterComponent(Component* component);
	void PreRender();

	void OnScreenResize(int32 width, int32 height);

	// TODO : 
	// ID를 바탕으로 오브젝트를 찾을 수 있는 방안을 마련해야함.
	// 빠른 오브젝트 탐색 방안을 마련해야 한다.

/*============================================//
//   essential render pass getter & setters   //
//============================================*/
// TODO : 이들을 한데 묶어 SRP로 둘 예정
public:
	auto& GetRenderPasses() const { return m_renderPasses; }
	RenderPass* GetRenderPass(const std::string& name);
	void SetShadowPass(ShadowPassUPtr pass) { m_shadowPass = std::move(pass); }
	ShadowPass* GetShadowPass() const { return m_shadowPass.get(); }
	void SetSkyboxPass(SkyboxPassUPtr pass) { m_skyboxPass = std::move(pass); }
	SkyboxPass* GetSkyboxPass() const { return m_skyboxPass.get(); }
	void SetPostProcessPass(PostProcessPassUPtr pass) { m_postProcessPass = std::move(pass); }
	PostProcessPass* GetPostProcessPass() const { return m_postProcessPass.get(); }
	void SetGeoemtryPass(GeometryPassUPtr pass) { m_geometryPass = std::move(pass); }
	GeometryPass* GetGeometryPass() const { return m_geometryPass.get(); }
	void SetDeferredLightingPass(DeferredLightingPassUPtr pass) { m_deferredLightPass = std::move(pass); }
	DeferredLightingPass* GetDeferredLightingPass() const { return m_deferredLightPass.get(); }

/*=======================================//
//   scene property getter and setters   //
//=======================================*/
public:
	const std::vector<Light*>& GetLights() const { return m_lights; }
	const std::vector<Camera*>& GetAllCameras() const { return m_cameras; }
	Camera* GetMainCamera() const { return m_mainCamera; }
	void SetMainCamera(Camera* camera) { m_mainCamera = camera; }

	// TEMP : 디버그 전용
	Light* GetMainLight() const { return m_mainLight; }
	void SetMainLight(Light* light) { m_mainLight = light; }

	// TEMP : 하늘 텍스쳐
	void SetSkyboxTexture(CubeTexturePtr texture) { m_skyboxTexture = texture; }
	CubeTexture* GetSkyboxTexture() const { return m_skyboxTexture.get(); }

protected:
	Scene();

	// 씬의 컨텍스트를 작성하는 가상 함수들
	virtual bool LoadNessesaryResources()	   = 0;
	virtual bool CreateNessesaryRenderPasses() = 0;
	virtual bool CreateSceneContext()          = 0;
	bool Init();

	// 모든 게임 오브젝트를 소유
	std::vector<GameObjectUPtr> m_gameObjects;

	// 씬 가비지 콜렉팅
	// TODO : GameObject 파괴를 위한 FlushDestroyQueue 구현 진행
	void FlushDestroyQueue();
	std::vector<GameObjectUPtr> m_destroyQueue;

	// TEMP : 씬의 디폴트 속성들
	Camera* m_mainCamera		{ nullptr };
	
	// TEMP : 디버그 전용 조명 -> 디버그 전용으로 따로 뺄 필요가 있음
	Light* m_mainLight			{ nullptr };

	// 컴포넌트 업데이트를 위한 참조 포인터
	std::vector<Light*>			m_lights;
	std::vector<Camera*>		m_cameras;
	std::vector<Animator*>		m_animators;
	std::vector<Script*>        m_scripts;

	// 하늘 텍스쳐
	CubeTexturePtr				m_skyboxTexture;

	// 렌더링 멤버들 : 일반 렌더링과 고정적으로 필요한 렌더링
	// TODO : 이들을 한데 묶어 SRP로 둘 예정
#pragma region STANDARD_RENDER_PIPELINE
	std::unordered_map<std::string, RenderPassUPtr> m_renderPasses;
	ShadowPassUPtr				m_shadowPass		{ nullptr };
	SkyboxPassUPtr				m_skyboxPass		{ nullptr };
	PostProcessPassUPtr			m_postProcessPass	{ nullptr };
	GeometryPassUPtr			m_geometryPass	    { nullptr };
	DeferredLightingPassUPtr    m_deferredLightPass { nullptr };

	UniformbufferUPtr m_cameraUBO;
	UniformbufferUPtr m_lightUBO;
	UniformbufferUPtr m_shadowUBO;
	int32 CreateRenderUBOs(); // TEMP : UBO 테스트 중
#pragma endregion
};