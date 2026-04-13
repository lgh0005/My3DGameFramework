#pragma once
#include "Registries/GameObjectRegistry.h"
// #include "Registries/ComponentRegistry.h"

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

	public:
		void Clear();
		GameObjectRegistry* GetGameObjectRegistry();
		// ComponentRegistry* GetComponentRegistry() const;

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
		template<typename T> T* AddComponent(ObjectIDHash ownerID);
		template<typename T> T* GetComponent(ObjectIDHash ownerID);

	private:
		GameObjectRegistry m_gameObjectRegistry;
		// ComponentRegistry m_componentRegistry;
	};
}

#include "Managers/EntityManager.inl"