#include "FrameworkPch.h"
#include "ScriptManager.h"
#include "Components/Script/Script.h"

namespace MGF3D
{
	ScriptManager::ScriptManager() = default;
	ScriptManager::~ScriptManager() = default;

	bool ScriptManager::Init()
	{
		m_scriptRegistry.Init();
		return true;
	}

	void ScriptManager::Update()
	{
		// 1. 레지스트리 단에서 지연 처리
		m_scriptRegistry.Update();

		// 2. 스크립트 실행 루프
		const auto& scripts = m_scriptRegistry.GetScripts();
		for (const auto& script : scripts)
		{
			// 빈자리(재사용 대기열)는 건너뜀
			if (script == nullptr) continue;

			auto state = script->GetState();

			// 꺼져 있거나 파괴 중인 스크립트는 로직 스킵
			if (state == EObjectLifecycle::Inactive ||
				state == EObjectLifecycle::PendingKill ||
				state == EObjectLifecycle::Dead) continue;

			// 첫 Update 루프 진입 시 Start 1회 호출
			if (state == EObjectLifecycle::PendingStart)
			{
				script->Start();
				script->SetState(EObjectLifecycle::Active);
			}

			// 완벽한 활성 상태일 때만 Update 호출
			if (script->GetState() == EObjectLifecycle::Active)
				script->Update();
		}
	}

	void ScriptManager::FixedUpdate()
	{
		// 물리 업데이트 등은 Flush 없이 순수 활성 스크립트만 돌립니다.
		for (const auto& script : m_scriptRegistry.GetScripts())
		{
			if (script && script->GetState() == EObjectLifecycle::Active)
				script->FixedUpdate();
		}
	}

	void ScriptManager::LateUpdate()
	{
		// 렌더링 직전 카메라 이동 처리 등에 사용됩니다.
		for (const auto& script : m_scriptRegistry.GetScripts())
		{
			if (script && script->GetState() == EObjectLifecycle::Active)
				script->LateUpdate();
		}
	}

	void ScriptManager::Shutdown()
	{
		m_scriptRegistry.Clear();
	}

	void ScriptManager::AddScript(ScriptUPtr script)
	{
		m_scriptRegistry.AddScript(std::move(script));
	}

	Script* ScriptManager::GetScript(ObjectIDHash ownerID)
	{
		return m_scriptRegistry.GetScript(ownerID);
	}

	void ScriptManager::RemoveScript(ObjectIDHash ownerID)
	{
		m_scriptRegistry.RemoveScript(ownerID);
	}
}