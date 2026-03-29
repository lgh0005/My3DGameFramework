#pragma once
#include "Pointer/PoolAlloc.h"
#include <thread>
#include "Containers/Slab/SString.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFThread)
	class MGFThread : public PoolAlloc
	{
		MGF_DISABLE_COPY(MGFThread)
		MGF_DISABLE_MOVE(MGFThread)

	public:
		MGFThread();
		~MGFThread();

	public:
		bool Start(const SString& name, Action<> work);
		bool Start(const SString& name, PAction<> work);

		void Join();
		void Detach();

		bool IsJoinable() const { return m_thread.joinable(); }
		const SString& GetName() const { return m_name; }

	private:
		Thread m_thread;
		SString m_name;
	};
}