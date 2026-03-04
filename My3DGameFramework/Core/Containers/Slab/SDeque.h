#include "Containers/TDeque.h"
#include "Memory/SlabAllocator.h"

namespace MGF3D
{
	template<typename T>
	using SDeque = TDeque<T, SlabAllocator<T>>;
}