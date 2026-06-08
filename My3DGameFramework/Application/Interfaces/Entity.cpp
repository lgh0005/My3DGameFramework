#include "ApplicationPch.h"
#include "Entity.h"
#include "Managers/ConsoleManager.h"

namespace MGF3D
{
	ObjectIDHash Entities::Create(const String& name)
	{
		MGF_CONSOLE.LogInfo("Creating GameObject with name: " + name);
		return MGF_ENTITY.CreateGameObject(name);
	}

	void Entities::Destroy(const String& name)
	{
		MGF_ENTITY.DestroyGameObject(name);
		MGF_CONSOLE.LogInfo("Destroyed GameObject with name: " + name);
	}

	void Entities::Destroy(ObjectIDHash id)
	{
		MGF_ENTITY.DestroyGameObject(id);
		MGF_CONSOLE.LogInfo("Destroyed GameObject with ID: " + std::to_string(id));
	}

	GameObject* Entities::Get(const String& name)
	{
		return MGF_ENTITY.GetGameObject(name);
	}

	GameObject* Entities::Get(ObjectIDHash id)
	{
		return MGF_ENTITY.GetGameObject(id);
	}
}