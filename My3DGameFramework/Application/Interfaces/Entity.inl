#pragma once

namespace MGF3D
{
	template<typename T, typename... Args>
	inline T* Entities::AddComponent(ObjectIDHash ownerID, Args&&... args)
	{
		return MGF_ENTITY.AddComponent<T>(ownerID, std::forward<Args>(args)...);
	}

	template<typename T>
	inline T* Entities::GetComponent(ObjectIDHash ownerID)
	{
		return MGF_ENTITY.GetComponent<T>(ownerID);
	}
}