#pragma once

namespace MGF3D
{
	template<typename T>
	inline void SVector<T>::Assign(usize n, const T& value)
	{
		this->assign(n, value);
	}

	template<typename T>
	inline void SVector<T>::Reserve(usize n)
	{
		this->reserve(n);
	}

	template<typename T>
	inline bool SVector<T>::Contains(const T& value) const
	{
		return std::find(this->begin(), this->end(), value) != this->end();
	}

	template<typename T>
	inline void SVector<T>::RemoveSwap(usize index)
	{
		if (index < this->size())
		{
			if (index != this->size() - 1) (*this)[index] = std::move(this->back());
			this->pop_back();
		}
	}
}