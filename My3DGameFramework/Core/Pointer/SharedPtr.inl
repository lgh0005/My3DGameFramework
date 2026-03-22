#pragma once
#include "Managers/MemoryManager.h"

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

	/*=========================================//
	//   default SharedPtr move constructors   //
	//=========================================*/
	template<typename T>
	inline SharedPtr<T>::SharedPtr(SharedPtr&& other) : m_ptr(other.m_ptr)
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