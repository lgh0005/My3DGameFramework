#include "CorePch.h"
#include "Task.h"

namespace MGF3D
{
	Task::Task(Action<> work, Action<> onComplete)
		: m_work(std::move(work))
		, m_onComplete(std::move(onComplete)) { }

	Task::~Task() = default;

	void Task::Execute()
	{
		// 1. 실제 메인 작업 수행
		if (m_work) m_work();

		// 2. 작업 완료 후 후속 조치 (콜백)
		// 보통 비동기 로딩에서는 여기서 "준비 완료" 신호를 보냅니다.
		// 쉽게 말해서 "나 작업 다했다"는 로직을 여기에다가 보내는 것이 일반적인
		// 로직이 될 것임.
		if (m_onComplete) m_onComplete();
	}
}