#pragma once

namespace MGF3D
{
    class CollisionBuilder
    {
    public:
        struct LayerEntry { SString Name; SString BPName; };
        struct RuleEntry { SString A; SString B; bool ShouldCollide; };

    public:
        void AddBroadPhase(const SString& inName);
        void AddObjectLayer(const SString& inName, const SString& inBPName);
        void SetRule(const SString& inA, const SString& inB, bool inShouldCollide);

    public:
        void Build();

    private:
        void BakeBroadPhases();
        void BakeObjectLayers();
        void ApplyRules();
        void Cleanup();

    private:
        SVector<SString>    m_PendingBPs;
        SVector<LayerEntry> m_PendingLayers;
        SVector<RuleEntry>  m_PendingRules;
    };
}
