#include "CorePch.h"
#include "TaskQueue.h"

namespace MGF3D
{
	TaskQueue::TaskQueue() = default;
	TaskQueue::~TaskQueue() = default;

	void TaskQueue::Push(Task&& task)
	{
		{
			LockScope lock(m_mutex);
			m_tasks.push_back(std::move(task));
		}
		m_cv.notify_one();
	}

	bool TaskQueue::WaitAndPop(Task& outTask)
	{
		UniqueLock lock(m_mutex);
		m_cv.wait(lock, [this]() { return m_bStop || !m_tasks.empty(); });

		if (m_bStop && m_tasks.empty())
			return false;

		outTask = std::move(m_tasks.front());
		m_tasks.pop_front();
		return true;
	}

	void TaskQueue::Stop()
	{
		{
			LockScope lock(m_mutex);
			m_bStop = true;
		}
		m_cv.notify_all();
	}
}