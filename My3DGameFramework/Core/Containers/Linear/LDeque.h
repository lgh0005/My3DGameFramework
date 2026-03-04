#include "Containers/TDeque.h"
#include "Memory/LinearAllocator.h"

namespace MGF3D
{
	template<typename T>
	using LDeque = TDeque<T, LinearAllocator<T>>;
}