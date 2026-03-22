#include "TBitset.h"
#pragma once

namespace MGF3D
{
	template<typename Alloc> inline TBitset<Alloc>::TBitset() = default;

	template<typename Alloc>
	inline TBitset<Alloc>::TBitset(usize inBitCount)
	{
		m_bits.Assign(inBitCount, false);
	}

	template<typename Alloc>
	inline void TBitset<Alloc>::Resize(usize inBitCount)
	{
		m_bits.Assign(inBitCount, false);
	}

	template<typename Alloc>
	inline void TBitset<Alloc>::Set(usize inIndex, bool inValue)
	{
		MGF_ASSERT(inIndex < m_bits.Count(), "Index out of range in TBitset::Set!");
		m_bits[inIndex] = inValue;
	}

	template<typename Alloc>
	inline bool TBitset<Alloc>::Test(usize inIndex) const
	{
		MGF_ASSERT(inIndex < m_bits.Count(), "Index out of range in TBitset::Set!");
		return m_bits[inIndex];
	}

	template<typename Alloc>
	inline void TBitset<Alloc>::Clear()
	{
		m_bits.Clear();
	}

	template<typename Alloc>
	inline usize TBitset<Alloc>::Size() const
	{
		return m_bits.Count();
	}

	template<typename Alloc>
	inline void TBitset<Alloc>::Release()
	{
		m_bits.Release();
	}

	template<typename Alloc>
	inline int32 TBitset<Alloc>::FindFirstSet() const
	{
		usize count = m_bits.Count();
		for (usize i = 0; i < count; ++i)
		{
			if (m_bits[i]) 
				return static_cast<int32>(i);
		}
		return -1;
	}

	template<typename Alloc>
	inline int32 TBitset<Alloc>::FindFirstUnset() const
	{
		usize count = m_bits.Count();
		for (usize i = 0; i < count; ++i)
		{
			if (!m_bits[i]) 
				return static_cast<int32>(i);
		}
		return -1;
	}

	template<typename Alloc>
	inline typename TBitset<Alloc>::reference TBitset<Alloc>::operator[](usize inIndex)
	{
		MGF_ASSERT(inIndex < m_bits.Count(), "Index out of range!");
		return m_bits[inIndex];
	}

	template<typename Alloc>
	inline bool TBitset<Alloc>::operator[](usize inIndex) const
	{
		MGF_ASSERT(inIndex < m_bits.Count(), "Index out of range!");
		return m_bits[inIndex];
	}

	template<typename Alloc>
	inline TBitset<Alloc>& TBitset<Alloc>::operator~()
	{
		for (auto&& bit : m_bits) bit = !bit;
		return *this;
	}

	template<typename Alloc>
	inline TBitset<Alloc> TBitset<Alloc>::operator!() const
	{
		TBitset<Alloc> result = *this;
		~result;
		return result;
	}
}