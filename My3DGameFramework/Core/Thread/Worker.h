#pragma once
#include "Thread/TaskQueue.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Worker)
	class Worker
	{
		MGF_DISABLE_COPY(Worker)

	public:
		Worker(TaskQueue& queue);
		virtual ~Worker();

	public:
		void Start();
		void Join();
		Thread::id GetID() const;

	protected:
		virtual void Run() = 0;

		uint32 m_id;
		TaskQueue& m_queue;
		Thread m_thread;
	};
}