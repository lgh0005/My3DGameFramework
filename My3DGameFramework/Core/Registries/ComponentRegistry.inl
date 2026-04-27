#pragma once
#include "Entities/Component.h"

namespace MGF3D
{
	template<typename T> inline ComponentRegistry<T>::ComponentRegistry() = default;
	template<typename T> inline ComponentRegistry<T>::~ComponentRegistry() = default;

	template<typename T>
	inline void ComponentRegistry<T>::Init()
	{
		m_pendingAdds.reserve(128);
		m_pendingKills.reserve(128);
	}

	template<typename T>
	inline void ComponentRegistry<T>::Update()
	{
		FlushPendingAdds();
		FlushPendingKills();
	}

	template<typename T>
	inline void ComponentRegistry<T>::Shutdown()
	{
		Clear();
	}

	template<typename T>
	inline void ComponentRegistry<T>::Clear()
	{
		m_pendingAdds.clear();
		m_pendingKills.clear();
		m_components.Clear();
	}

	/*==================================//
	//    Component API                 //
	//==================================*/
	template<typename T>
	template<typename... Args>
	inline T* ComponentRegistry<T>::AddComponent(ObjectIDHash ownerID, Args&&... args)
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Component, T>,
			"T must be derived from Component."
		);

		if (!ownerID.IsValid()) return nullptr;
		int32 index = ownerID.GetIndex();

		if (m_components.Has(index))
			return m_components.Get(index);

		T* newComponent = &m_components.Emplace(index, ownerID, ownerID, std::forward<Args>(args)...);
		m_pendingAdds.push_back(ownerID);
		return newComponent;
	}

	template<typename T>
	inline T* ComponentRegistry<T>::GetComponent(ObjectIDHash ownerID)
	{
		if (!ownerID.IsValid()) return nullptr;
		return m_components.Get(ownerID.GetIndex());
	}

	template<typename T>
	inline void ComponentRegistry<T>::RemoveComponent(ObjectIDHash ownerID)
	{
		if (!ownerID.IsValid()) return;
		if (m_components.Has(ownerID.GetIndex()))
			m_pendingKills.push_back(ownerID);
	}

	template<typename T>
	inline void ComponentRegistry<T>::FlushPendingAdds()
	{
		if (m_pendingAdds.empty()) return;

		for (ObjectIDHash ownerID : m_pendingAdds)
		{
			int32 index = ownerID.GetIndex();
			T* comp = m_components.Get(index);
			if (comp)
			{
				comp->SetState(EObjectLifecycle::Active);
				m_components.MoveToFrontGroup(index);
			}
		}
		m_pendingAdds.clear();
	}

	template<typename T>
	inline void ComponentRegistry<T>::FlushPendingKills()
	{
		if (m_pendingKills.empty()) return;

		for (ObjectIDHash ownerID : m_pendingKills)
		{
			int32 index = ownerID.GetIndex();
			if (m_components.Has(index)) 
				m_components.Remove(index);
		}
		m_pendingKills.clear();
	}
}