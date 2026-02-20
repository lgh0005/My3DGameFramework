#pragma once

namespace MGF3D
{
	class IContainer
	{
	public:
		IContainer();
		virtual ~IContainer();

	public:
		IContainer(const IContainer&) = default;
		IContainer& operator=(const IContainer&) = default;
		IContainer(IContainer&&) noexcept = default;
		IContainer& operator=(IContainer&&) noexcept = default;

	public:
		// 힙 할당 관련 연산자들 삭제
		static void* operator new(usize) = delete;
		static void* operator new[](usize) = delete;
		static void* operator new(usize, alignment) = delete;
		static void* operator new[](usize, alignment) = delete;

		// 힙 할당 반환 관련 연산자들 삭제
		static void operator delete(void*) = delete;
		static void operator delete[](void*) = delete;

	/*==============================//
	//   common container methods   //
	//==============================*/
	public:
		virtual usize MemoryUsage() const = 0;
		virtual usize Count() const = 0;
		virtual bool Empty() const = 0;
		virtual void Clear() = 0;
	};
}