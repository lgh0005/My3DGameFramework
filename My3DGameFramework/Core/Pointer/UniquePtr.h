#pragma once

namespace MGF3D
{
	template<typename T>
	struct DefaultDeleter
	{
		void operator()(T* ptr) const
		{
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