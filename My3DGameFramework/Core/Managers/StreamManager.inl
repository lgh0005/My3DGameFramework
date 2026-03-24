#pragma once
#include "Task/MGFAsyncResult.h"
#include "Managers/TaskManager.h"

namespace MGF3D
{
	template<typename T>
	inline void StreamManager::ReadFileAsync
	(
		const MGFPath& path, 
		Func<Nullable<T>, FileStreamPtr> processor, 
		Action<Nullable<T>> onComplete)
	{
		auto result = MakeShared<MGFAsyncResult<T>>();

		auto task = MGF_TASK.AcquireTask
		(
			/* [Worker Thread] */
			[this, path, result, processor]()
			{
				// [Worker Thread] 핸들만 엽니다.
				auto streamResult = OpenRead(path);
				if (streamResult.IsNull())
				{
					result->Set(None);
					return;
				}

				// [Worker Thread] 열린 파일 스트림을 바로 파싱
				result->Set(processor(streamResult.Get()));
			},

			/* [Main Thread] */
			[result, onComplete = std::move(onComplete)]() mutable
			{
				if (onComplete)
					onComplete(std::move(result->Get()));
			}
		);

		MGF_TASK.PushTask(std::move(task));
	}

	template<typename T>
	inline void StreamManager::ReadBytesAsync
	(
		const MGFPath& path, 
		Func<Nullable<T>, MemoryStreamPtr> processor, 
		Action<Nullable<T>> onComplete
	)
	{
		auto result = MakeShared<MGFAsyncResult<T>>();

		auto task = MGF_TASK.AcquireTask
		(
			[this, path, result, processor]()
			{
				// [Worker Thread] 전체 데이터를 메모리에 붓습니다.
				auto streamResult = ReadAllBytes(path);
				if (streamResult.IsNull())
				{
					result->Set(None);
					return;
				}

				// [Worker Thread] 메모리에 올라온 데이터를 가공합니다.
				result->Set(processor(streamResult.Get()));
			},
			[result, onComplete = std::move(onComplete)]() mutable
			{
				if (onComplete)
					onComplete(std::move(result->Get()));
			}
		);

		MGF_TASK.PushTask(std::move(task));
	}
}