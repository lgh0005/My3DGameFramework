#pragma once
#include "Thread/Worker.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFWindow)

	MGF_CLASS_PTR(GPUWorker)
	class GPUWorker : public Worker
	{
		using Super = Worker;

	public:
		GPUWorker(TaskQueue& queue, MGFWindowUPtr sharedWindow);
		virtual ~GPUWorker() override;

	protected:
		virtual void Run() override;

	private:
		MGFWindowUPtr m_sharedWindow;
	}; 
}