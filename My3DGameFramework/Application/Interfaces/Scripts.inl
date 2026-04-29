#pragma once

namespace MGF3D
{
	template<typename T>
	inline T* Scripts::AddScript(ObjectIDHash ownerID)
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Script, T>,
			"T must be derived from Script."
		);
		if (!ownerID.IsValid()) return nullptr;

		auto newScript = MakeUnique<T>(ownerID, ownerID);
		T* scriptPtr = newScript.get();
		MGF_SCRIPT.AddScript(std::move(newScript));

		return scriptPtr;
	}

	template<typename T>
	inline T* Scripts::GetScript(ObjectIDHash ownerID)
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Script, T>,
			"T must be derived from Script."
		);
		if (!ownerID.IsValid()) return nullptr;

		Script* baseScript = MGF_SCRIPT.GetScript(ownerID);
		return MGFTypeCaster::Cast<T*>(baseScript);
	}

	inline void Scripts::Remove(ObjectIDHash ownerID)
	{
		if (ownerID.IsValid()) MGF_SCRIPT.RemoveScript(ownerID);
	}
}