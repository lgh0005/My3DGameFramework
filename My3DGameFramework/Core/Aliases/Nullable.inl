#pragma once

namespace MGF3D
{
	template<typename T>
	inline Nullable<T>::Nullable() = default;

	template<typename T>
	inline Nullable<T>::Nullable(none_t) : m_value(None) { }

	template<typename T>
	inline Nullable<T>::Nullable(const T& value) : m_value(value) { }

	template<typename T>
	inline Nullable<T>::Nullable(T&& value) : m_value(std::move(value)) { }
}