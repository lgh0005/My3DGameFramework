#pragma once

namespace MGF3D
{
	template <typename T>
	class SharedPtr
	{
	/*====================================//
	//   default SharedPtr constructors   //
	//====================================*/
	public:
		SharedPtr();
		SharedPtr(null);
		explicit SharedPtr(T* ptr);
		~SharedPtr();

	/*=========================================//
	//   default SharedPtr copy constructors   //
	//=========================================*/
	public:
		SharedPtr(const SharedPtr& other);
		SharedPtr& operator=(const SharedPtr& other) noexcept;

	/*=========================================//
	//   default SharedPtr move constructors   //
	//=========================================*/
	public:
		SharedPtr(SharedPtr&& other);
		SharedPtr& operator=(SharedPtr&& other) noexcept;

	/*==========================================//
	//   default SharedPtr pointer operations   //
	//==========================================*/
	public:
		T* operator->() const;
		T& operator*() const;

	/*==========================================//
	//   default SharedPtr boolean operations   //
	//==========================================*/
	public:
		explicit operator bool() const;
		bool operator==(const SharedPtr& other) const;
		bool operator!=(const SharedPtr& other) const;

	/*==========================================//
	//      default SharedPtr management        //
	//==========================================*/
	public:
		T* Get() const { return m_ptr; }

	private:
		T* m_ptr	{ nullptr };
	};
}

#include "Pointer/SharedPtr.inl"