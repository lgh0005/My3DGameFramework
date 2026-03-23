#pragma once

namespace MGF3D
{
	/*====================================//
	//   default UniquePtr constructors   //
	//====================================*/
	template<typename T>
	inline UniquePtr<T>::UniquePtr() : m_ptr(nullptr) {}

	template<typename T>
	inline UniquePtr<T>::UniquePtr(null) : m_ptr(nullptr) {}

	template<typename T>
	inline UniquePtr<T>::UniquePtr(T* ptr) : m_ptr(ptr) {}

	template<typename T>
	inline UniquePtr<T>::~UniquePtr()
	{
		Reset();
	}

	/*=========================================//
	//   default UniquePtr move constructors   //
	//=========================================*/
	template<typename T>
	inline UniquePtr<T>::UniquePtr(UniquePtr&& other) noexcept
		: m_ptr(other.m_ptr)
	{
		other.m_ptr = nullptr;
	}

	template<typename T>
	inline UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr&& other) noexcept
	{
		if (this != &other) Reset(other.Release());
		return *this;
	}


	template<typename T>
	template<typename U> // U* 에서 T* 로의 암시적 업캐스팅 발생 
	inline UniquePtr<T>::UniquePtr(UniquePtr<U>&& other) noexcept
		: m_ptr(other.Release()) { }

	template<typename T>
	template<typename U>
	inline UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr<U>&& other) noexcept
	{
		// 템플릿 타입이 다르므로 자기 자신인지(this != &other) 비교할 필요가 사실상 없으나, 
		// 소유권 이전의 명확성을 위해 Reset을 활용합니다.
		Reset(other.Release());
		return *this;
	}

	/*==========================================//
	//   default UniquePtr pointer operations   //
	//==========================================*/
	template<typename T>
	inline T* UniquePtr<T>::operator->() const
	{
		MGF_ASSERT(m_ptr != nullptr, "UniquePtr: Accessing null pointer via ->");
		return m_ptr;
	}

	template<typename T>
	inline T& UniquePtr<T>::operator*() const
	{
		MGF_ASSERT(m_ptr != nullptr, "UniquePtr: Dereferencing null pointer via *");
		return *m_ptr;
	}

	/*==========================================//
	//   default UniquePtr boolean operations   //
	//==========================================*/
	template<typename T>
	inline UniquePtr<T>::operator bool() const
	{
		return m_ptr != nullptr;
	}

	template<typename T>
	inline bool UniquePtr<T>::operator==(const UniquePtr& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T>
	inline bool UniquePtr<T>::operator!=(const UniquePtr& other) const
	{
		return m_ptr != other.m_ptr;
	}

	/*==========================================//
	//      default UniquePtr management        //
	//==========================================*/
	template<typename T>
	inline T* UniquePtr<T>::Release()
	{
		T* ptr = m_ptr;
		m_ptr = nullptr;
		return ptr;
	}

	template<typename T>
	inline void UniquePtr<T>::Reset(T* ptr)
	{
		if (m_ptr != ptr)
		{
			if (m_ptr) delete m_ptr;
			m_ptr = ptr;
		}
	}
}