#pragma once
#include "Entities/Entity/ObjectStateMachine.h"

namespace MGF3D
{
	enum class ComponentState : uint8
	{
		None,
		PendingAdd,		 // 생성되어 Storage 등록 대기 중
		Alive,			 // Storage에 등록되어 정상 동작 가능한 상태
		PendingDestroy,	 // 삭제 큐에 담긴 상태
		Dead			 // 완전히 소멸됨
	};

	class ComponentStateMachine : public ObjectStateMachine
	{
		using Super = ObjectStateMachine;

	public:
		ComponentStateMachine(Ptr<Object> owner);
		virtual ~ComponentStateMachine() override;

	public:
		void SetState(ComponentState newState) { m_state = newState; }
		ComponentState GetState() const { return m_state; }
		void SetEnabled(bool enabled);
		bool IsEnabled() const { return m_enabled; }

	public:
		// Storage에서 업데이트 여부를 판단할 때 호출
		virtual bool IsActive() const override;
		virtual bool IsDead() const override;

	private:
		ComponentState m_state{ ComponentState::None };
		bool m_enabled{ true };
	};
}