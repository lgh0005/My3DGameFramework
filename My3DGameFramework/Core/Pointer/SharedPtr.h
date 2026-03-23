#pragma once

namespace MGF3D
{
	template <typename T>
	class SharedPtr
	{
		// TODO : T는 RefCount 클래스의 상속이어야 한는 것을 STATIC_ASSERT
		
		template<typename U> friend class SharedPtr;
		// TODO : U는 RefCount 클래스의 상속이어야 한는 것을 STATIC_ASSERT

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
		template<typename U> SharedPtr(const SharedPtr<U>& other);
		template<typename U> SharedPtr& operator=(const SharedPtr<U>& other) noexcept;

	/*=========================================//
	//   default SharedPtr move constructors   //
	//=========================================*/
	public:
		SharedPtr(SharedPtr&& other) noexcept;
		SharedPtr& operator=(SharedPtr&& other) noexcept;
		template<typename U> SharedPtr(SharedPtr<U>&& other) noexcept;
		template<typename U> SharedPtr& operator=(SharedPtr<U>&& other) noexcept;

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