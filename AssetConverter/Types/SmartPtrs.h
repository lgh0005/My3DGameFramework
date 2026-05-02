#pragma once
#include <memory>

namespace MGF3D
{
	// 스마트 포인터 타입 정의
	template <typename T> using UniquePtr = std::unique_ptr<T>;
	template <typename T> using SharedPtr = std::shared_ptr<T>;
	template <typename T> using WeakPtr = std::weak_ptr<T>;

	// unique 포인터 생성 함수
	template <typename T, typename... Args>
	constexpr UniquePtr<T> MakeUnique(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	// shared_ptr 포인터 생성 함수
	template <typename T, typename... Args>
	constexpr SharedPtr<T> MakeShared(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
