#pragma once

namespace MGF3D
{
	template<typename T, typename... Args>
	Ref<T> MakeRef(Args&&... args);
}

#include "Pointer/JoltRefFactory.inl"