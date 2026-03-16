#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(JoltMemoryPool)

	class JoltMemoryManager
	{
		MGF_DECLARE_SINGLE(JoltMemoryManager)

	private:
		JoltMemoryManager();
		~JoltMemoryManager();

	public:

		// TODO : 메모리와 관련된 상수들은 따로 빼둘 필요가 있음
		bool Init(usize inTempCapacity = 10 * 1024 * 1024);
		void Shutdown();
		Ptr<JPH::TempAllocator> GetTempAllocator() const;

	private:
		Ptr<JoltMemoryPool> m_tempPool	{nullptr};
	};
}