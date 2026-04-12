#include "RuntimePch.h"
#include "GameObjectRegistry.h"

namespace MGF3D
{
	GameObjectRegistry::GameObjectRegistry() = default;
	GameObjectRegistry::~GameObjectRegistry() = default;

	void GameObjectRegistry::Init()
	{

	}

	void GameObjectRegistry::Update()
	{

	}

	void GameObjectRegistry::Shutdown()
	{

	}

	void GameObjectRegistry::Clear()
	{

	}

	ObjectIDHash GameObjectRegistry::AddGameObject(const GameObjectInfo& info)
	{
		return ObjectIDHash();
	}

	void GameObjectRegistry::DestroyGameObject(ObjectIDHash id)
	{

	}

	void GameObjectRegistry::FlushPendingAdds()
	{

	}

	void GameObjectRegistry::FlushPendingKills()
	{

	}

	ObjectIDHash GameObjectRegistry::GenerateNewId()
	{
		return ObjectIDHash();
	}

	void GameObjectRegistry::ReleaseId(ObjectIDHash id)
	{

	}
}