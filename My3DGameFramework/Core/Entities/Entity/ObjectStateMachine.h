#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Object)

	MGF_CLASS_PTR(ObjectStateMachine)
	class ObjectStateMachine : public PoolAlloc
	{
	public:
		ObjectStateMachine(Ptr<Object> owner);
		virtual ~ObjectStateMachine();

	/*==========================================//
	//   Abstractions for Engine Systems        //
	//==========================================*/
	public:

		// Storage의 Update 루프에서 이 객체를 돌릴지 말지 결정
		virtual bool IsActive() const = 0;

		// 펜딩 큐에서 제거하고 메모리를 해제해도 되는 상태인지 확인
		virtual bool IsDead() const = 0;

	protected:
		Ptr<Object> m_owner	{ nullptr };
	};
}