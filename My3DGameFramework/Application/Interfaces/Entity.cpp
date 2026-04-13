#include "ApplicationPch.h"
#include "Entity.h"

namespace MGF3D
{
	ObjectIDHash Entities::Create(const String& name)
	{
		return MGF_ENTITY.CreateGameObject(name);
	}

	void Entities::Destroy(const String& name)
	{
		MGF_ENTITY.DestroyGameObject(name);
	}

	void Entities::Destroy(ObjectIDHash id)
	{
		MGF_ENTITY.DestroyGameObject(id);
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