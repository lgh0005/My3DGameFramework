#include "CorePch.h"
#include "GPUWorker.h"
#include "Window/MGFWindow.h"

namespace MGF3D
{
	GPUWorker::GPUWorker(TaskQueue& queue, MGFWindowUPtr sharedWindow)
		: Super(queue), m_sharedWindow(std::move(sharedWindow)) { }
	GPUWorker::~GPUWorker() = default;

	void GPUWorker::Run()
	{
		// 1. 스레드가 시작되자마자 자신의 전용 공유 윈도우를 현재 컨텍스트로 생성
		if (m_sharedWindow && m_sharedWindow->GetHandle())
			glfwMakeContextCurrent(m_sharedWindow->GetHandle());

		Task task;
		while (m_queue.WaitAndPop(task))
		{
			if (task)
			{
				task();
				if (m_sharedWindow) glFlush();
			}
		}
	}
}