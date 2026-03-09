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
		// 1. 상태 변경 시에만 짧게 락을 겁니다.
		{
			MGF_LOCK_SCOPE(m_mutex);
			m_signaled = true;
		}

		// 2. 락을 해제한 후 신호를 보냅니다 (Wait-Wakeup 병목 방지).
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