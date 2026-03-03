#pragma once
#include "Task/MGFFuture.h"
#include "Task/MGFPromise.h"

namespace MGF3D
{
	class AsyncTask
	{
	public:
		template<typename T>
		static MGFFuture<T> Run(Func<T> work);
		static void Run(Action<> work, Action<> onComplete = nullptr);
	};
}

#include "Task/AsyncTask.inl"
