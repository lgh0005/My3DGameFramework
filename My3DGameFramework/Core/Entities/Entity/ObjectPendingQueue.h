#pragma once
#include "Entities/Entity/Object.h"

namespace MGF3D
{
	class ObjectPendingQueue
	{
	public:
		void PushCreate(Ptr<Object>obj);
		void PushDestroy(Ptr<Object> obj);

	public:
		void FlushCreateQueue();
		void FlushDestroyQueue();

	private:
		SVector<Ptr<Object>> m_pendingCreateQueue;
		SVector<Ptr<Object>> m_pendingDestroyQueue;
	};
}