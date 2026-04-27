#pragma once
#include "Registries/GameObjectRegistry.h"
#include "Registries/ComponentRegistry.h"

namespace MGF3D
{
	class EntityManager
	{
		MGF_DECLARE_SINGLE(EntityManager)

	private:
		EntityManager();
		~EntityManager();

	public:
		void Init();
		void Update();
		void Shutdown();

	/*==========================//
	//     Registry methods     //
	//==========================*/
	public:
		void Clear();
		GameObjectRegistry* GetGameObjectRegistry();
		void AddComponentRegistry(int16 typeIndex, IComponentRegistryUPtr registry);
		template<typename T> ComponentRegistry<T>* GetComponentRegistry() const;

	/*============================//
	//     GameObject methods     //
	//============================*/
	public:
		ObjectIDHash CreateGameObject(const String& name);
		GameObject* GetGameObject(const String& name);
		void DestroyGameObject(const String& name);

		GameObject* GetGameObject(ObjectIDHash id);
		void DestroyGameObject(ObjectIDHash id);

	/*===========================//
	//     Component methods     //
	//===========================*/
	public:
		template<typename T, typename... Args>
		T* AddComponent(ObjectIDHash ownerID, Args&&... args);
		template<typename T> T* GetComponent(ObjectIDHash ownerID);

	private:
		GameObjectRegistry m_gameObjectRegistry;
		Vector<IComponentRegistry*> m_registryLookup;
		Vector<IComponentRegistryUPtr> m_componentRegistries;
	};
}

#include "Managers/EntityManager.inl"