#pragma once

namespace MGF3D
{
	/*====================================//
	//   default SharedPtr constructors   //
	//====================================*/
	template<typename T> inline SharedPtr<T>::SharedPtr() : m_ptr(nullptr) { }
	template<typename T> inline SharedPtr<T>::SharedPtr(null) : m_ptr(nullptr) { }
	
	template<typename T>
	inline SharedPtr<T>::SharedPtr(T* ptr) : m_ptr(ptr)
	{
		if (m_ptr) m_ptr->IncRef();
	}

	template<typename T>
	inline SharedPtr<T>::~SharedPtr()
	{
		if (m_ptr) m_ptr->DecRef();
	}

	/*=========================================//
	//   default SharedPtr copy constructors   //
	//=========================================*/
	template<typename T>
	inline SharedPtr<T>::SharedPtr(const SharedPtr& other) : m_ptr(other.m_ptr)
	{
		if (m_ptr) m_ptr->IncRef();
	}

	template<typename T>
	inline SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) noexcept
	{
		if (this != &other)
		{
			if (m_ptr) m_ptr->DecRef();
			m_ptr = other.m_ptr;
			if (m_ptr) m_ptr->IncRef();
		}
		return *this;
	}

	// 파생 클래스 포인터에서 기반 클래스 포인터로의 암시적 변환
	template<typename T>
	template<typename U>
	inline SharedPtr<T>::SharedPtr(const SharedPtr<U>& other) : m_ptr(other.m_ptr)
	{
		if (m_ptr) m_ptr->IncRef();
	}

	template<typename T>
	template<typename U>
	inline SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<U>& other) noexcept
	{
		// 같은 객체를 가리키고 있지 않은 경우에만 처리
		if (m_ptr != other.m_ptr)
		{
			if (m_ptr) m_ptr->DecRef(); // 기존 소유 자원 해제 및 카운트 감소
			m_ptr = other.m_ptr;
			if (m_ptr) m_ptr->IncRef();
		}
		return *this;
	}

	/*=========================================//
	//   default SharedPtr move constructors   //
	//=========================================*/
	template<typename T>
	inline SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept : m_ptr(other.m_ptr)
	{
		other.m_ptr = nullptr;
	}

	template<typename T>
	inline SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ptr) m_ptr->DecRef();
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
		}
		return *this;
	}

	template<typename T>
	template<typename U>
	inline SharedPtr<T>::SharedPtr(SharedPtr<U>&& other) noexcept
		: m_ptr(other.m_ptr)
	{
		other.m_ptr = nullptr; // 카운트 조작 없이 소유권(참조)만 훔쳐옵니다.
	}

	template<typename T>
	template<typename U>
	inline SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) noexcept
	{
		if (m_ptr != other.m_ptr)
		{
			if (m_ptr) m_ptr->DecRef();
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
		}
		return *this;
	}

	/*==========================================//
	//   default SharedPtr pointer operations   //
	//==========================================*/
	template<typename T>
	inline T* SharedPtr<T>::operator->() const
	{
		MGF_ASSERT(m_ptr != nullptr, "SharedPtr: Accessing null pointer via ->");
		return m_ptr;
	}

	template<typename T>
	inline T& SharedPtr<T>::operator*() const
	{
		MGF_ASSERT(m_ptr != nullptr, "SharedPtr: Dereferencing null pointer via *");
		return *m_ptr;
	}

	/*==========================================//
	//   default SharedPtr boolean operations   //
	//==========================================*/
	template<typename T>
	inline SharedPtr<T>::operator bool() const
	{
		return m_ptr != nullptr;
	}

	template<typename T>
	inline bool SharedPtr<T>::operator==(const SharedPtr& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T>
	inline bool SharedPtr<T>::operator!=(const SharedPtr& other) const
	{
		return m_ptr != other.m_ptr;
	}
}