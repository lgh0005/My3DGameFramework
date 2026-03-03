#include "EnginePch.h"
#include "SceneRegistry.h"
#include "Scene/ComponentRegistry.h"
#include "Scene/GameObjectRegistry.h"

DECLARE_DEFAULTS_IMPL(SceneRegistry)

SceneRegistryUPtr SceneRegistry::Create()
{
	auto registry = SceneRegistryUPtr(new SceneRegistry());
	if (!registry->Init()) return nullptr;
	return std::move(registry);
}

bool SceneRegistry::Init()
{
	m_componentRegistry = ComponentRegistry::Create();
	m_gameObjectRegistry = GameObjectRegistry::Create();
	return (m_componentRegistry && m_gameObjectRegistry);
}

ComponentRegistry* SceneRegistry::GetComponentRegistry() const
{
	return m_componentRegistry.get();
}

GameObjectRegistry* SceneRegistry::GetGameObjectRegistry() const
{
	return m_gameObjectRegistry.get();
}
