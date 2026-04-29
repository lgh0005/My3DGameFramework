#pragma once
#include "Registries/ScriptRegistry.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Script)

	class ScriptManager
	{
		MGF_DECLARE_SINGLE(ScriptManager)

	private:
		ScriptManager();
		~ScriptManager();

	public:
		bool Init();
		void FixedUpdate();
		void Update();
		void LateUpdate();
		void Shutdown();

	public:
		void AddScript(ScriptUPtr script);
		Script* GetScript(ObjectIDHash ownerID);
		void RemoveScript(ObjectIDHash ownerID);

	private:
		ScriptRegistry m_scriptRegistry;
	};
}