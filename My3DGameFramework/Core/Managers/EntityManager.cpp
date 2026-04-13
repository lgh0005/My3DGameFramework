#include "CorePch.h"
#include "EntityManager.h"

namespace MGF3D
{
	EntityManager::EntityManager() = default;
	EntityManager::~EntityManager() = default;

	void EntityManager::Init()
	{
		m_gameObjectRegistry.Init();
		// m_componentRegistry.Init();
	}

	void EntityManager::Update()
	{
		m_gameObjectRegistry.Update();
		// m_componentRegistry.Update();
	}

	void EntityManager::Shutdown()
	{
		m_gameObjectRegistry.Shutdown();
		// m_componentRegistry.Shutdown();
	}

	void EntityManager::Clear()
	{
		m_gameObjectRegistry.Clear();
		// m_componentRegistry.Clear();
	}

	GameObjectRegistry* EntityManager::GetGameObjectRegistry()
	{
		return &m_gameObjectRegistry;
	}

	//ComponentRegistry* EntityManager::GetComponentRegistry() const
	//{
	//	// return &m_componentRegistry;
	//	return nullptr;
	//}

	/*============================//
	//     GameObject methods     //
	//============================*/
	ObjectIDHash EntityManager::CreateGameObject(const String& name)
	{
		return m_gameObjectRegistry.AddGameObject(name);
	}

	GameObject* EntityManager::GetGameObject(const String& name)
	{
		return m_gameObjectRegistry.GetGameObject(name);
	}

	void EntityManager::DestroyGameObject(const String& name)
	{
		m_gameObjectRegistry.DestroyGameObject(name);
	}

	GameObject* EntityManager::GetGameObject(ObjectIDHash id)
	{
		return m_gameObjectRegistry.GetGameObject(id);
	}

	void EntityManager::DestroyGameObject(ObjectIDHash id)
	{
		m_gameObjectRegistry.DestroyGameObject(id);
	}
}