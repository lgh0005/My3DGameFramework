#pragma once
#include "Registries/IComponentRegistry.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Script)

	class ScriptRegistry : public IComponentRegistry
	{
	public:
		ScriptRegistry();
		virtual ~ScriptRegistry() override;

	public:
		virtual void Init() override;
		virtual void Update() override;
		virtual void Shutdown() override;
		virtual void Clear() override;

	public:
		void AddScript(ScriptUPtr script);
		Script* GetScript(ObjectIDHash ownerID);
		void RemoveScript(ObjectIDHash ownerID);
		const Vector<ScriptUPtr>& GetScripts() const;

	private:
		void FlushPendingAdds();
		void FlushPendingKills();

	private:
		Vector<ScriptUPtr> m_scripts;
		Vector<usize> m_freeSlots;
		HashMap<ObjectIDHash, Vector<usize>> m_ownerToIndices;

		Vector<ScriptUPtr> m_pendingAdds;
		Vector<ObjectIDHash> m_pendingKills;
	};
}