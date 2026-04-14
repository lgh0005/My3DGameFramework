#pragma once

namespace MGF3D
{
	/*===========================//
	//     Component methods     //
	//===========================*/
	template<typename T>
	inline T* EntityManager::AddComponent(ObjectIDHash ownerID)
	{
		usize typeIdx = T::s_typeIndex;
		if (typeIdx >= m_registryLookup.size() || m_registryLookup[typeIdx] == nullptr)
			return nullptr;

		auto* registry = static_cast<ComponentRegistry<T>*>(m_registryLookup[typeIdx]);
		return registry->AddComponent(ownerID);
	}

	template<typename T>
	inline T* EntityManager::GetComponent(ObjectIDHash ownerID)
	{
		usize typeIdx = T::s_typeIndex;
		if (typeIdx >= m_registryLookup.size() || m_registryLookup[typeIdx] == nullptr)
			return nullptr;

		auto* registry = static_cast<ComponentRegistry<T>*>(m_registryLookup[typeIdx]);
		return registry->GetComponent(ownerID);
	}

	template<typename T>
	inline ComponentRegistry<T>* EntityManager::GetComponentRegistry() const
	{
		usize typeIdx = T::s_typeIndex;
		if (typeIdx >= m_registryLookup.size() || m_registryLookup[typeIdx] == nullptr)
			return nullptr;
		return static_cast<ComponentRegistry<T>*>(m_registryLookup[typeIdx]);
	}
}