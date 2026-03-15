#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(JoltFilterSet)

	class CollisionManager
	{
		MGF_DECLARE_SINGLE(CollisionManager)
		friend class CollisionBuilder;

	private:
		CollisionManager();
		~CollisionManager();

	public:
		void Init();
		void Clear();
		void Shutdown();

		usize GetObjectLayerCount() const { return m_ObjectNameToID.Count(); }
		usize GetBPLayerCount()     const { return m_BroadPhaseNameToID.Count(); }
		Ptr<JoltFilterSet> GetFilter() const { return m_filter.get(); }

		bool ShouldCollideObjects(JPH::ObjectLayer inL1, JPH::ObjectLayer inL2) const;
		bool ShouldCollideBroadPhase(JPH::ObjectLayer inLayer, JPH::BroadPhaseLayer inBP) const;
		JPH::BroadPhaseLayer GetBroadPhaseOfLayer(JPH::ObjectLayer inLayer) const;
		cstr GetBroadPhaseName(JPH::BroadPhaseLayer inBP) const;

	public:
		void SetLayerCollision(const SString& inLayerA, const SString& inLayerB, bool inShouldCollide);
		void SetBroadPhaseCollision(const SString& inLayer, const SString& inBP, bool inShouldCollide);

	private:
		SMap<StringHash, JPH::ObjectLayer>      m_ObjectNameToID;
		SMap<StringHash, JPH::BroadPhaseLayer>  m_BroadPhaseNameToID;
		SVector<SString>                     m_BroadPhaseIDToName;

		SVector<JPH::BroadPhaseLayer>        m_ObjectToBP;
		SVector<SBitset>                     m_ObjectVsBPMatrix;
		SVector<SBitset>                     m_ObjectVsObjectMatrix;

		JoltFilterSetUPtr m_filter;
	};
}