#pragma once

namespace MGF3D
{
	class JoltAllocator
	{
	public:
		static void* Allocate(usize inSize);
		static void  Free(void* inBlock);

		static void* AlignedAllocate(usize inSize, usize inAlignment);
		static void  AlignedFree(void* inBlock);

		static void* Reallocate(void* inBlock, usize inOldSize, usize inNewSize);
	};
}