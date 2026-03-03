#include "CorePch.h"
#include "MGFSignal.h"

namespace MGF3D
{
	MGFSignal::MGFSignal() : m_signaled(false) {}
	MGFSignal::~MGFSignal()
	{
		Set();
	}

	void MGFSignal::Set()
	{
		MGF_LOCK_SCOPE
		{
			Lock lock(m_mutex);
			m_signaled = true;
		}

		m_cv.notify_one();
	}

	void MGFSignal::Wait()
	{
		UniqueLock lock(m_mutex);

		// 신호가 올 때까지 대기
		m_cv.wait(lock, [this] { return m_signaled; });

		// 깨어나는 순간 즉시 끄기 (Auto-Reset)
		m_signaled = false;
	}
}