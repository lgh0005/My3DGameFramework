#pragma once
#include "Thread/Worker.h"

namespace MGF3D
{
	MGF_CLASS_PTR(CPUWorker)
	class CPUWorker : public Worker
	{
		using Super = Worker;

	public:
		CPUWorker(TaskQueue& queue);
		virtual ~CPUWorker() override;

	protected:
		virtual void Run() override;
	};
}
