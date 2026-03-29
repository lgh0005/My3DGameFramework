#pragma once

namespace MGF3D
{
	class PoolAlloc
	{
	public:
		static void* operator new(usize size) noexcept;
		static void* operator new[](usize size) noexcept;
		static void operator delete(void* ptr, usize size) noexcept;
		static void operator delete[](void* ptr, usize size) noexcept;
	};
}