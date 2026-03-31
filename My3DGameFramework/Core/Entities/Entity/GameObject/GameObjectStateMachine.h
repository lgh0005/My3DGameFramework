#pragma once
#include "Entities/Entity/ObjectStateMachine.h"

namespace MGF3D
{
	enum class GameObjectState : uint8
	{
		None,
		PendingAdd,		 // 생성되어 큐에 담긴 상태 (아직 Update 대상 아님)
		Alive,           // 스토리지에 등록되어 살아있는 상태 (Active/Inactive 전환 가능)
		PendingDestroy,  // 삭제 대기 중 (삭제 큐에 담기거나 담길 예정)
		Dead             // 완전히 소멸됨
	};

	class GameObjectStateMachine : public ObjectStateMachine
	{
		using Super = ObjectStateMachine;

	public:
		GameObjectStateMachine(Ptr<Object> owner);
		virtual ~GameObjectStateMachine() override;

	public:
		void SetState(GameObjectState newState) { m_state = newState; }
		GameObjectState GetState() const { return m_state; }
		void SetActive(bool active);
		bool IsActive() const;
		bool IsDead() const;
		
	public:
		// TODO : 이후 복합적인 상태 관리 필요

	private:
		GameObjectState m_state { GameObjectState::None };
		bool m_active	{ true };
	};
}