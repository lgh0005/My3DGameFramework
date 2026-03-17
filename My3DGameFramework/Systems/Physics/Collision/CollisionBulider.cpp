#include "PhysicsPch.h"
#include "CollisionBulider.h"
#include "Managers/CollisionManager.h"

namespace MGF3D
{
	void CollisionBuilder::AddBroadPhase(const SString& inName)
	{
		for (const auto& bp : m_PendingBPs)
		{
			if (bp == inName)
			{
				MGF_LOG_WARN
                (
                    "[Physics] BroadPhase layer '{0}' is already registered. Skipping duplicate entry.", 
                    inName.CStr()
                );
				return;
			}
		}

		m_PendingBPs.PushBack(inName);
	}

	void CollisionBuilder::AddObjectLayer(const SString& inName, const SString& inBPName)
	{
		m_PendingLayers.PushBack({ inName, inBPName });
	}

	void CollisionBuilder::SetRule(const SString& inA, const SString& inB, bool inShouldCollide)
	{
		m_PendingRules.PushBack({ inA, inB, inShouldCollide });
	}

	void CollisionBuilder::Build()
	{
        MGF_ASSERT(MGF_COLLISION.GetFilter() != nullptr, "CollisionManager::Init() must be called before Build()!");

        MGF_LOG_INFO("[Collision] Starting data-driven collision system build...");

        // 1. Reset existing data
        MGF_COLLISION.Clear();

        // 2~4. Perform baking process
        BakeBroadPhases();
        BakeObjectLayers();
        ApplyRules();

        // 5. Cleanup temporary builder data
        Cleanup();

        MGF_LOG_INFO("[Collision] Build completed successfully.");
	}

    void CollisionBuilder::BakeBroadPhases()
    {
        const uint32 bpCount = (uint32)m_PendingBPs.Count();
        MGF_COLLISION.m_BroadPhaseIDToName.Resize(bpCount);

        for (uint32 i = 0; i < bpCount; ++i)
        {
            const SString& bpName = m_PendingBPs[i];
            MGF_COLLISION.m_BroadPhaseNameToID[bpName.Hash()] = JPH::BroadPhaseLayer((JPH::BroadPhaseLayer::Type)i);
            MGF_COLLISION.m_BroadPhaseIDToName[i] = bpName;
        }
    }

    void CollisionBuilder::BakeObjectLayers()
    {
        const uint32 bpCount = (uint32)m_PendingBPs.Count();
        const uint32 objCount = (uint32)m_PendingLayers.Count();

        MGF_COLLISION.m_ObjectToBP.Resize(objCount);
        MGF_COLLISION.m_ObjectVsBPMatrix.Resize(objCount);
        MGF_COLLISION.m_ObjectVsObjectMatrix.Resize(objCount);

        for (uint32 i = 0; i < objCount; ++i)
        {
            const auto& layerDef = m_PendingLayers[i];
            MGF_COLLISION.m_ObjectNameToID[layerDef.Name.Hash()] = (JPH::ObjectLayer)i;

            // 소속 BP 찾기
            auto* pBP = MGF_COLLISION.m_BroadPhaseNameToID.Find(layerDef.BPName.Hash());
            MGF_ASSERT(pBP != nullptr, "Using an unregistered BroadPhase name in layer definition!");
            MGF_COLLISION.m_ObjectToBP[i] = *pBP;

            // 비트셋 공간 확보 (Baking)
            MGF_COLLISION.m_ObjectVsBPMatrix[i].Resize(bpCount);
            MGF_COLLISION.m_ObjectVsObjectMatrix[i].Resize(objCount);
        }
    }

    void CollisionBuilder::ApplyRules()
    {
        for (const auto& rule : m_PendingRules)
        {
            const StringHash hashA = rule.A.Hash();
            const StringHash hashB = rule.B.Hash();

            const bool isObjA = MGF_COLLISION.m_ObjectNameToID.Contains(hashA);
            const bool isObjB = MGF_COLLISION.m_ObjectNameToID.Contains(hashB);
            const bool isBPB = MGF_COLLISION.m_BroadPhaseNameToID.Contains(hashB);

            // Case: Object Layer vs Object Layer
            if (isObjA && isObjB)
            {
                MGF_COLLISION.SetLayerCollision(rule.A, rule.B, rule.ShouldCollide);
            }
            // Case: Object Layer vs BroadPhase Layer
            else if (isObjA && isBPB)
            {
                MGF_COLLISION.SetBroadPhaseCollision(rule.A, rule.B, rule.ShouldCollide);
            }
        }
    }

    void CollisionBuilder::Cleanup()
    {
        m_PendingBPs.Release();
        m_PendingLayers.Release();
        m_PendingRules.Release();
    }
}