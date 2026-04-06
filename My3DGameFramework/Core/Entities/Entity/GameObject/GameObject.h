#pragma once
#include "Entities/Entity/Object.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Component)
	MGF_CLASS_PTR(GameObjectStateMachine)

	class GameObject : public Object
	{
	public:
		virtual ~GameObject() override;
		using ComponentCache = SMap<Ptr<const MGFType>, Ptr<Component>>;
		Ptr<GameObjectStateMachine> GetGameObjectStateMachine() const;
		static Ptr<GameObject> Create();

	public:
		template<typename T, typename... Args> 
		T* AddComponent(Args&&... args);
		
		template<typename T> 
		T* GetComponent() const;

	public:
		void RegisterComponent(Ptr<Component> component);
		const ComponentCache& GetComponents() const;

	/*====================================//
	//   GameObject getters and setters   //
	//====================================*/
	public:
		void SetStorageIndex(usize index) { m_storageIndex = index; }
		usize GetStorageIndex() const { return m_storageIndex; }
		void SetTransformIndex(usize index) { m_transformIndex = index; }
		usize GetTransformIndex() const { return m_transformIndex; }

	/*================================//
	//   GameObject state utilities   //
	//================================*/
	public:
		void SetActive(bool active);
		bool IsActive() const;

	private:
		GameObject();
		Ptr<GameObject> Init(UniquePtr<GameObject>&& self);
		usize m_transformIndex{ static_cast<usize>(-1) };
		ComponentCache m_components;
		usize		   m_storageIndex{ 0 };
	};
}

#include "Entities/Entity/GameObject/GameObject.inl"
