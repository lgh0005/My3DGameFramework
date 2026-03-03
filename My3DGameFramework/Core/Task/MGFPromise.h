#pragma once
#include <future>
#include "Thread/MGFFuture.h"

namespace MGF3D
{
	template<typename T>
	class MGFPromise
	{
	public:
		MGFPromise();
		MGFFuture<T> GetFuture();
		void SetValue(const T& value);
		void SetValue(T&& value);

	private:
		Promise<T> m_promise;
	};
}

#include "Thread/MGFPromise.inl"