#pragma once

namespace MGF3D
{
	template<typename T>
	inline T* Entities::AddComponent(ObjectIDHash ownerID)
	{
		return MGF_ENTITY.AddComponent<T>(ownerID);
	}

	template<typename T>
	inline T* Entities::GetComponent(ObjectIDHash ownerID)
	{
		return MGF_ENTITY.GetComponent<T>(ownerID);
	}
}