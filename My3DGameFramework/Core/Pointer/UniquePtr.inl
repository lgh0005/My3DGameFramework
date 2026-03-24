#pragma once

namespace MGF3D
{
	/*====================================//
	//   default UniquePtr constructors   //
	//====================================*/
	template<typename T, typename Deleter>
	inline UniquePtr<T, Deleter>::UniquePtr()
		: m_ptr(nullptr), m_deleter(Deleter()) { }

	template<typename T, typename Deleter>
	inline UniquePtr<T, Deleter>::UniquePtr(null)
		: m_ptr(nullptr), m_deleter(Deleter()) { }

	template<typename T, typename Deleter>
	inline UniquePtr<T, Deleter>::UniquePtr(T* ptr)
		: m_ptr(ptr), m_deleter(Deleter()) { }

	template<typename T, typename Deleter>
	inline UniquePtr<T, Deleter>::UniquePtr(T* ptr, const Deleter& deleter)
		: m_ptr(ptr), m_deleter(deleter) { }

	template<typename T, typename Deleter>
	inline UniquePtr<T, Deleter>::~UniquePtr()
	{
		Reset();
	}

	/*=========================================//
	//   default UniquePtr move constructors   //
	//=========================================*/
	template<typename T, typename Deleter>
	inline UniquePtr<T, Deleter>::UniquePtr(UniquePtr&& other) noexcept
		: m_ptr(other.m_ptr), m_deleter(std::move(other.m_deleter))
	{
		other.m_ptr = nullptr;
	}

	template<typename T, typename Deleter>
	inline UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr&& other) noexcept
	{
		if (this != &other)
		{
			// 현재 소유한 자원을 먼저 정리하고 소유권 이전
			Reset();
			m_ptr = other.Release();
			m_deleter = std::move(other.m_deleter);
		}
		return *this;
	}

	template<typename T, typename Deleter>
	template<typename U, typename E>
	inline UniquePtr<T, Deleter>::UniquePtr(UniquePtr<U, E>&& other) noexcept
		: m_ptr(other.Release()), m_deleter(std::move(other.GetDeleter())) { }

	template<typename T, typename Deleter>
	template<typename U, typename E>
	inline UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr<U, E>&& other) noexcept
	{
		// 템플릿 타입이 다르므로 자기 자신인지(this != &other) 비교할 필요가 사실상 없으나, 
		// 소유권 이전의 명확성을 위해 Reset을 활용합니다.
		Reset(other.Release());
		m_deleter = std::move(other.GetDeleter());
		return *this;
	}

	/*==========================================//
	//   default UniquePtr pointer operations   //
	//==========================================*/
	template<typename T, typename Deleter>
	inline T* UniquePtr<T, Deleter>::operator->() const
	{
		MGF_ASSERT(m_ptr != nullptr, "UniquePtr: Accessing null pointer via ->");
		return m_ptr;
	}

	template<typename T, typename Deleter>
	inline T& UniquePtr<T, Deleter>::operator*() const
	{
		MGF_ASSERT(m_ptr != nullptr, "UniquePtr: Dereferencing null pointer via *");
		return *m_ptr;
	}

	/*==========================================//
	//   default UniquePtr boolean operations   //
	//==========================================*/
	template<typename T, typename Deleter>
	inline UniquePtr<T, Deleter>::operator bool() const
	{
		return m_ptr != nullptr;
	}

	template<typename T, typename Deleter>
	inline bool UniquePtr<T, Deleter>::operator==(const UniquePtr& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T, typename Deleter>
	inline bool UniquePtr<T, Deleter>::operator!=(const UniquePtr& other) const
	{
		return m_ptr != other.m_ptr;
	}

	/*==========================================//
	//      default UniquePtr management        //
	//==========================================*/
	template<typename T, typename Deleter>
	inline T* UniquePtr<T, Deleter>::Release()
	{
		T* ptr = m_ptr;
		m_ptr = nullptr;
		return ptr;
	}

	template<typename T, typename Deleter>
	inline void UniquePtr<T, Deleter>::Reset(T* ptr)
	{
		if (m_ptr != ptr)
		{
			if (m_ptr) m_deleter(m_ptr);
			m_ptr = ptr;
		}
	}
}