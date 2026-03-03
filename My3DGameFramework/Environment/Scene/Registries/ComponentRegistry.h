#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(GameObject)
	MGF_CLASS_PTR(Component)
	MGF_CLASS_PTR(GeneralRenderPass)

	MGF_CLASS_PTR(ComponentRegistry)
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
		SkyLight* GetSkyLight(uint32 idx) const;
		void AddCustomRenderPass(const std::string& name, GeneralRenderPassUPtr pass);
		GeneralRenderPass* GetCustomRenderPass(const std::string& name);
		CustomPassMap& GetCustomRenderPasses();

	private:
		ComponentCache m_componentCache;
		CustomPassMap m_customPasses;
	};
}

#include "Scene/ComponentRegistry.inl"