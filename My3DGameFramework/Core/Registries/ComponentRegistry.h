#pragma once
#include "Registries/IComponentRegistry.h"

namespace MGF3D
{
	template<typename T>
	class ComponentRegistry : public IComponentRegistry
	{
	public:
		ComponentRegistry();
		virtual ~ComponentRegistry() override;

	public:
		virtual void Init() override;
		virtual void Update() override;
		virtual void Shutdown() override;
		virtual void Clear() override;

	public:
		T* AddComponent(ObjectIDHash ownerID);
		T* GetComponent(ObjectIDHash ownerID);
		void RemoveComponent(ObjectIDHash ownerID);

	private:
		void FlushPendingAdds();
		void FlushPendingKills();

	private:
		MGFPackedArray<T> m_components;

		Vector<ObjectIDHash> m_pendingAdds;
		Vector<ObjectIDHash> m_pendingKills;
	};
}

#include "Registries/ComponentRegistry.inl"