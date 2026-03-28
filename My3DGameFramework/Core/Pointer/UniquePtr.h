#pragma once

namespace MGF3D
{
	template<typename T>
	struct DefaultDeleter
	{
		DefaultDeleter() = default;

		// 1. [복사 생성] 다른 타입(U)의 Deleter로부터 생성 허용
		template <typename U>
		DefaultDeleter(const DefaultDeleter<U>&) noexcept {}

		// 2. [이동 생성] rvalue로 넘어오는 Deleter 대응 (inl 56라인 대응)
		template <typename U>
		DefaultDeleter(DefaultDeleter<U>&&) noexcept {}

		// 3. [복사 대입] 
		template <typename U>
		DefaultDeleter& operator=(const DefaultDeleter<U>&) noexcept { return *this; }

		// 4. [이동 대입]
		template <typename U>
		DefaultDeleter& operator=(DefaultDeleter<U>&&) noexcept { return *this; }

		void operator()(T* ptr) const
		{
			// 부모 소멸자가 virtual이어야 자식까지 안전하게 죽습니다.
			if (ptr) delete ptr;
		}
	};

	template<typename T, typename Deleter = DefaultDeleter<T>>
	class UniquePtr
	{
	/*====================================//
	//   default UniquePtr constructors   //
	//====================================*/
	public:
		UniquePtr();
		UniquePtr(null);
		explicit UniquePtr(T* ptr);
		UniquePtr(T* ptr, const Deleter& deleter);
		~UniquePtr();

	/*=========================================//
	//   default UniquePtr copy constructors   //
	//=========================================*/
		MGF_DISABLE_COPY(UniquePtr)

	/*=========================================//
	//   default UniquePtr move constructors   //
	//=========================================*/
	public:
		UniquePtr(UniquePtr&& other) noexcept;
		UniquePtr& operator=(UniquePtr&& other) noexcept;
		template<typename U, typename E> UniquePtr(UniquePtr<U, E>&& other) noexcept;
		template<typename U, typename E> UniquePtr& operator=(UniquePtr<U, E>&& other) noexcept;

	/*==========================================//
	//   default UniquePtr pointer operations   //
	//==========================================*/
	public:
		T* operator->() const;
		T& operator*() const;

	/*==========================================//
	//   default UniquePtr boolean operations   //
	//==========================================*/
	public:
		explicit operator bool() const;
		bool operator==(const UniquePtr& other) const;
		bool operator!=(const UniquePtr& other) const;

	/*==========================================//
	//      default UniquePtr management        //
	//==========================================*/
	public:
		T* Get() const { return m_ptr; }
		Deleter& GetDeleter() { return m_deleter; }
		const Deleter& GetDeleter() const { return m_deleter; }
		T* Release();
		void Reset(T* ptr = nullptr);

	private:
		T* m_ptr	{ nullptr };
		Deleter m_deleter;
	};
}

#include "Pointer/UniquePtr.inl"