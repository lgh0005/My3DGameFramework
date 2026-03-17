#include "CorePch.h"
#include "CollisionManager.h"
#include "Collision/JoltFilterSet.h"

namespace MGF3D
{
	CollisionManager::CollisionManager()  = default;
	CollisionManager::~CollisionManager() = default;

	bool CollisionManager::Init()
	{
		// 통합 필터 생성
		m_filter = MakeUnique<JoltFilterSet>();
		MGF_ASSERT(m_filter != nullptr, "CollisionManager: Failed to allocate JoltFilterSet memory!");
		
		MGF_LOG_INFO("[Collision] CollisionManager Initialized Successfully.");
		return true;
	}

	void CollisionManager::Shutdown()
	{
		m_filter.reset();
		m_ObjectNameToID.Release();
		m_BroadPhaseNameToID.Release();
		m_BroadPhaseIDToName.Release();
		m_ObjectToBP.Release();
		m_ObjectVsBPMatrix.Release();
		m_ObjectVsObjectMatrix.Release();
	}

	void CollisionManager::Clear()
	{
		m_ObjectNameToID.Clear();
		m_BroadPhaseNameToID.Clear();
		m_BroadPhaseIDToName.Clear();
		m_ObjectToBP.Clear();
		m_ObjectVsBPMatrix.Clear();
		m_ObjectVsObjectMatrix.Clear();
	}

	bool CollisionManager::ShouldCollideObjects(JPH::ObjectLayer inL1, JPH::ObjectLayer inL2) const
	{
		MGF_ASSERT(inL1 < m_ObjectVsObjectMatrix.Count(), "ObjectLayer index 1 is out of range!");
		MGF_ASSERT(inL2 < m_ObjectVsObjectMatrix.Count(), "ObjectLayer index 2 is out of range!");
		return m_ObjectVsObjectMatrix[inL1].Test(inL2);
	}

	bool CollisionManager::ShouldCollideBroadPhase(JPH::ObjectLayer inLayer, JPH::BroadPhaseLayer inBP) const
	{
		MGF_ASSERT(inLayer < m_ObjectVsBPMatrix.Count(), "ObjectLayer is out of range!");

		uint32 bpIndex = inBP.GetValue();
		MGF_ASSERT(bpIndex < m_BroadPhaseNameToID.Count(), "BroadPhaseLayer index is out of range!");

		return m_ObjectVsBPMatrix[inLayer].Test(bpIndex);
	}

	JPH::BroadPhaseLayer CollisionManager::GetBroadPhaseOfLayer(JPH::ObjectLayer inLayer) const
	{
		MGF_ASSERT(inLayer < m_ObjectToBP.Count(), "ObjectLayer is out of range!");
		return m_ObjectToBP[inLayer];
	}

	cstr CollisionManager::GetBroadPhaseName(JPH::BroadPhaseLayer inBP) const
	{
		const uint32 index = inBP.GetValue();
		if (index < m_BroadPhaseIDToName.Count()) return m_BroadPhaseIDToName[index].CStr();
		return "Unknown_BroadPhase";
	}

	void CollisionManager::SetLayerCollision(const SString& inLayerA, const SString& inLayerB, bool inShouldCollide)
	{
		// Find IDs from names
		auto* pIdA = m_ObjectNameToID.Find(inLayerA.Hash());
		auto* pIdB = m_ObjectNameToID.Find(inLayerB.Hash());

		if (pIdA && pIdB)
		{
			// Update bitset matrix symmetrically
			m_ObjectVsObjectMatrix[*pIdA].Set(*pIdB, inShouldCollide);
			m_ObjectVsObjectMatrix[*pIdB].Set(*pIdA, inShouldCollide);
		}
		else
		{
			MGF_LOG_WARN
			(
				"[Physics] Failed to find layers for SetLayerCollision: {0}, {1}", 
				inLayerA.CStr(),
				inLayerB.CStr()
			);
		}
	}

	void CollisionManager::SetBroadPhaseCollision(const SString& inLayer, const SString& inBP, bool inShouldCollide)
	{
		auto* pIdL = m_ObjectNameToID.Find(inLayer.Hash());
		auto* pIdBP = m_BroadPhaseNameToID.Find(inBP.Hash());

		if (pIdL && pIdBP)
		{
			// BroadPhaseLayer to uint32 (Value)
			uint32 bpIndex = pIdBP->GetValue();
			m_ObjectVsBPMatrix[*pIdL].Set(bpIndex, inShouldCollide);
		}
		else
		{
			MGF_LOG_WARN
			(
				"[Physics] Failed to find layers for SetBroadPhaseCollision: {0}, {1}", 
				inLayer.CStr(),
				inBP.CStr()
			);
		}
	}
}