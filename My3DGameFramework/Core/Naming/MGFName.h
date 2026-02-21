#pragma once
#include "Hashing/StringHash.h"

namespace MGF3D
{
	struct MGFName
	{
	public:
		MGFName();
		MGFName(cstr s);
		MGFName(strview sv);

	public:
		bool IsEmpty() const;
		cstr CStr() const;
		uint32 GetHashValue() const;

	public:
		bool operator==(const MGFName& other) const { return m_string == other.m_string; }
		bool operator!=(const MGFName& other) const { return m_string != other.m_string; }
		bool operator<(const MGFName& other) const { return m_hash < other.m_hash; }

	private:
		cstr       m_string; // StringManager의 풀링된 주소
		StringHash m_hash;   // 미리 계산된 해시값

	};
}