#pragma once

namespace MGF3D
{
	using Task = Action<>;

	MGF_CLASS_PTR(TaskQueue)
	class TaskQueue
	{
		MGF_DISABLE_COPY(TaskQueue)

	public:
		TaskQueue();
		~TaskQueue();

	public:

		// [생산자용] 일감을 넣고 알람을 울립니다.
		void Push(Task&& task);

		// [소비자용] 일감이 올 때까지 대기하다가 하나 꺼내옵니다.
		bool WaitAndPop(Task& outTask);

		// [매니저용] 영업 종료 선언
		void Stop();

	private:
		Queue<Task> m_tasks;
		Mutex m_mutex;
		ConditionVariable m_cv;
		Atomic<bool> m_bStop{ false };
	};
}