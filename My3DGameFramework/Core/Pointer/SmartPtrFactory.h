#pragma once

namespace MGF3D
{
	template<typename T, typename... Args> SharedPtr<T> MakeShared(Args&&... args);
	template<typename T, typename... Args> UniquePtr<T> MakeUnique(Args&&... args);
}

#include "Pointer/SmartPtrFactory.inl"