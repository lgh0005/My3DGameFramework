#pragma once
#include "Containers/TVector.h"

namespace MGF3D
{
	template<typename Alloc>
	class TBitset
	{
	public:
		TBitset();
		TBitset(usize inBitCount);

		void Resize(usize inBitCount);
		void Set(usize inIndex, bool inValue);
		bool Test(usize inIndex) const;
		void Clear();
		usize Size() const;
		void  Release();

	public:
		using reference = typename TVector<bool, Alloc>::reference;
		reference operator[](usize inIndex);
		bool operator[](usize inIndex) const;

		TBitset& operator~();
		TBitset  operator!() const;

	private:
		TVector<bool, Alloc> m_bits;
	};
}

#include "Containers/TBitset.inl"