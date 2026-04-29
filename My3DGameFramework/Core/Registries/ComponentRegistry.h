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
		template<typename... Args>
		T* AddComponent(ObjectIDHash ownerID, Args&&... args);
		T* GetComponent(ObjectIDHash ownerID);
		void RemoveComponent(ObjectIDHash ownerID);
		const MGFPackedArray<T>& GetComponents() const { return m_components; }

	protected:
		void FlushPendingAdds();
		void FlushPendingKills();

	protected:
		MGFPackedArray<T> m_components;

		Vector<ObjectIDHash> m_pendingAdds;
		Vector<ObjectIDHash> m_pendingKills;
	};
}

#include "Registries/ComponentRegistry.inl"