#include "CorePch.h"
#include "EntityManager.h"

namespace MGF3D
{
	EntityManager::EntityManager() = default;
	EntityManager::~EntityManager() = default;

	void EntityManager::Init()
	{
		m_gameObjectRegistry.Init();
		for (auto& registry : m_componentRegistries)
		{
			if (registry != nullptr)
				registry->Init();
		}
	}

	void EntityManager::Update()
	{
		m_gameObjectRegistry.Update();
		for (auto& registry : m_componentRegistries)
		{
			if (registry != nullptr)
				registry->Update();
		}
	}

	void EntityManager::Shutdown()
	{
		m_gameObjectRegistry.Shutdown();
		for (auto& registry : m_componentRegistries)
		{
			if (registry != nullptr)
				registry->Shutdown();
		}
	}

	void EntityManager::Clear()
	{
		m_gameObjectRegistry.Clear();
		for (auto& registry : m_componentRegistries)
		{
			if (registry != nullptr)
				registry->Clear();
		}
	}

	GameObjectRegistry* EntityManager::GetGameObjectRegistry()
	{
		return &m_gameObjectRegistry;
	}

	void EntityManager::AddComponentRegistry(int16 typeIndex, IComponentRegistryUPtr registry)
	{
		if (typeIndex < 0 || registry == nullptr) return;
		if (typeIndex >= m_registryLookup.size())
			m_registryLookup.resize(typeIndex + 1, nullptr);
		m_registryLookup[typeIndex] = registry.get();
		m_componentRegistries.push_back(std::move(registry));
	}

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