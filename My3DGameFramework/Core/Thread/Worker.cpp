#include "CorePch.h"
#include "Worker.h"

namespace MGF3D
{
	Worker::Worker(TaskQueue& queue) : m_queue(queue) { }
	Worker::~Worker()
	{
		Join();
	}

	void Worker::Start()
	{
		m_thread = Thread([this]() { Run(); });
	}

	void Worker::Join()
	{
		if (m_thread.joinable())
			m_thread.join();
	}

	Thread::id Worker::GetID() const
	{
		return m_thread.get_id();
	}
}