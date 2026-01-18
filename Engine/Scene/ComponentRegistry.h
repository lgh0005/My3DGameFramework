#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Component)
CLASS_PTR(GeneralRenderPass)
CLASS_PTR(SkyLight)
#pragma endregion

CLASS_PTR(ComponentRegistry)
class ComponentRegistry
{
public:
	~ComponentRegistry();
	static ComponentRegistryUPtr Create();

	using ComponentCache = std::array<std::vector<Component*>, (usize)ComponentType::MAX>;
	using CustomPassMap = std::unordered_map<std::string, GeneralRenderPassUPtr>;

	void RegisterComponent(Component* component);
	void UnregisterComponent(Component* component);
	template<typename T> const std::vector<Component*>& GetComponents();

private:
	void Init();
	ComponentRegistry();
	void RemoveComponentFromBaseList(ComponentType baseType, Component* component);

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

#include "Scene/ComponentRegistry.inl"