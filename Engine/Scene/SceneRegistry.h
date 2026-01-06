#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Component)
CLASS_PTR(StaticMeshRenderer)
CLASS_PTR(SkinnedMeshRenderer)
CLASS_PTR(InstancedMeshRenderer)
CLASS_PTR(MeshOutline)
CLASS_PTR(UICanvas)
CLASS_PTR(Light)
CLASS_PTR(Camera)
CLASS_PTR(Animator)
CLASS_PTR(Script)
CLASS_PTR(AudioSource)
CLASS_PTR(AudioListener)
CLASS_PTR(Collider)
CLASS_PTR(Rigidbody)
CLASS_PTR(GeneralRenderPass)
CLASS_PTR(SkyLight)
#pragma endregion

CLASS_PTR(SceneRegistry)
class SceneRegistry
{
public:
	~SceneRegistry();
	static SceneRegistryUPtr Create();

	void RegisterComponent(Component* component);
	void UnregisterComponent(Component* component);

private:
	SceneRegistry();

	template<typename T> 
	void RemoveFromVector(std::vector<T*>& vec, Component* comp);

/*=======================================//
//   getters and setters of components   //
//=======================================*/
public:
	Camera* GetCamera(uint32 idx) const;
	void AddCustomRenderPass(const std::string& name, GeneralRenderPassUPtr pass);
	GeneralRenderPass* GetCustomRenderPass(const std::string& name);
	auto& GetCustomRenderPasses() const { return m_customPasses; }
	void SetSkyLight(SkyLightUPtr skyLight);
	SkyLight* GetSkyLight() const;

public:
	const auto& GetCameras() const { return m_cameras; }
	const auto& GetLights() const { return m_lights; }
	const auto& GetAnimators() const { return m_animators; }
	const auto& GetScripts() const { return m_scripts; }
	const auto& GetAudioSources() const { return m_audioSources; }
	const auto& GetAudioListeners() const { return m_audioListeners; }
	const auto& GetStaticMeshRenderers() const { return m_staticMeshRenderers; }
	const auto& GetSkinnedMeshRenderers() const { return m_skinnedMeshRenderers; }
	const auto& GetInstancedMeshRenderers() const { return m_instancedMeshRenderers; }
	const auto& GetMeshOutlines() const { return m_outlines; }
	const auto& GetRigidbodies() const { return m_rigidBodies; }
	const auto& GetColliders() const { return m_colliders; }
	const auto& GetUICanvases() const { return m_uiCanvases; }

private:
	std::vector<Camera*>                m_cameras;
	std::vector<Light*>                 m_lights;
	std::vector<Animator*>              m_animators;
	std::vector<Script*>                m_scripts;
	std::vector<AudioSource*>           m_audioSources;
	std::vector<AudioListener*>         m_audioListeners;
	std::vector<Rigidbody*>				m_rigidBodies;
	std::vector<Collider*>				m_colliders;

	// 렌더링 관련
	std::vector<StaticMeshRenderer*>    m_staticMeshRenderers;
	std::vector<SkinnedMeshRenderer*>   m_skinnedMeshRenderers;
	std::vector<InstancedMeshRenderer*> m_instancedMeshRenderers;
	std::vector<MeshOutline*>           m_outlines;
	std::vector<UICanvas*>              m_uiCanvases;

	// 커스텀 렌더 패스와 스카이박스
	SkyLightUPtr				m_sky;
	std::unordered_map<std::string, GeneralRenderPassUPtr> m_customPasses;
};

/*====================================//
//   scene registry template inlines  //
//====================================*/
#pragma region SCENE_REGISTRY_TEMPLATE_INLINES
template<typename T>
inline void SceneRegistry::RemoveFromVector(std::vector<T*>& vec, Component* comp)
{
	// 삭제는 빈번하므로 Swap & Pop 패턴 적용 (순서 중요치 않음)
	T* target = static_cast<T*>(comp);

	// 1. target 찾기
	auto it = std::find(vec.begin(), vec.end(), target);

	// 2. 대상을 찾았다면 Swap & Pop 진행
	// INFO : 컴포넌트의 업데이트 순서는 중요하지 않으므로 Swap & Pop으로 삭제
	if (it != vec.end())
	{
		std::iter_swap(it, vec.end() - 1);
		vec.pop_back();
	}
}
#pragma endregion
