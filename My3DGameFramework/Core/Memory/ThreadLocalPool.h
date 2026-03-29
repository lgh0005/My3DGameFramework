#pragma once

namespace MGF3D
{
	class ThreadLocalPool
	{
		MGF_DECLARE_UTILITIES(ThreadLocalPool)

	public:
		static Ptr<LinearMemoryPool> Get() noexcept;

	private:

		// 스레드별 메모리 생명주기를 책임지는 내부 RAII 래퍼
		class TlsWrapper
		{
		public:
			TlsWrapper() noexcept;
			~TlsWrapper();
			Ptr<LinearMemoryPool> GetPool() const noexcept;

		private:
			byte* m_buffer;
			Ptr<LinearMemoryPool> m_pool;
		};
	};
}