#pragma once
#include "Managers/MemoryManager.h"

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
		if (this != &other)
		{
			Reset();
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
		}
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
		if (m_ptr)
		{
			m_ptr->~T();
			MGF_MEMORY.Deallocate(m_ptr, sizeof(T));
		}
		m_ptr = ptr;
	}
}