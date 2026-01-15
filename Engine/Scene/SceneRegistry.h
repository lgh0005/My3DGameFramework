#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Component)
CLASS_PTR(GeneralRenderPass)
CLASS_PTR(SkyLight)
#pragma endregion

CLASS_PTR(SceneRegistry)
class SceneRegistry
{
public:
	~SceneRegistry();
	static SceneRegistryUPtr Create();

	using ComponentCache = std::array<std::vector<Component*>, (usize)ComponentType::MAX>;
	using CustomPassMap = std::unordered_map<std::string, GeneralRenderPassUPtr>;

	void RegisterComponent(Component* component);
	void UnregisterComponent(Component* component);
	template<typename T> const std::vector<Component*>& GetComponents();

private:
	void Init();
	SceneRegistry();

/*=======================================//
//   getters and setters of components   //
//=======================================*/
public:
	Camera* GetCamera(uint32 idx) const;
	void AddCustomRenderPass(const std::string& name, GeneralRenderPassUPtr pass);
	GeneralRenderPass* GetCustomRenderPass(const std::string& name);
	CustomPassMap& GetCustomRenderPasses();
	void SetSkyLight(SkyLightUPtr skyLight);
	SkyLight* GetSkyLight() const;

private:
	ComponentCache m_componentCache;
	CustomPassMap m_customPasses;
	SkyLightUPtr m_sky;
};

/*====================================//
//   scene registry template inlines  //
//====================================*/
template<typename T>
inline const std::vector<Component*>& SceneRegistry::GetComponents()
{
	ComponentType type = T::GetStaticComponentType();
	return m_componentCache[(usize)type];
}