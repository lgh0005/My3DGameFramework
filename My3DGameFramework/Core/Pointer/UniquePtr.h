#pragma once

namespace MGF3D
{
	template <typename T>
	class UniquePtr
	{
	/*====================================//
	//   default UniquePtr constructors   //
	//====================================*/
	public:
		UniquePtr();
		UniquePtr(null);
		explicit UniquePtr(T* ptr);
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

	/*====================================================//
	//   default UniquePtr memory allocation operations   //
	//====================================================*/
	public:


	/*==========================================//
	//      default UniquePtr management        //
	//==========================================*/
	public:
		T* Get() const { return m_ptr; }
		T* Release();
		void Reset(T* ptr = nullptr);

	private:
		T* m_ptr{ nullptr };
	};
}

#include "Pointer/UniquePtr.inl"