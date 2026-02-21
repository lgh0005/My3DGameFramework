#pragma once
#include "Hashing/StringHash.h"

namespace MGF3D
{
	struct MGFName
	{
		friend class NameManager;

	public:
		// 1. 생성자 에서는 오로지 문자열만 받도록 만들고,
		MGFName();
		MGFName(cstr s, bool makeUnique = false);
		MGFName(strview sv, bool makeUnique = false);

	public:
		bool IsEmpty() const;
		cstr CStr() const;
		StringHash GetStringHash() const;

	public:
		bool operator==(const MGFName& other) const { return m_string == other.m_string; }
		bool operator!=(const MGFName& other) const { return m_string != other.m_string; }
		bool operator<(const MGFName& other) const { return m_hash < other.m_hash; }

	private:
		cstr       m_string;
		StringHash m_hash;

		void SetString(cstr s) { m_string = s; }
		void SetString(strview sv) { m_string = sv.data(); }

		void SetStringHash(StringHash h) { m_hash = h; }
		void SetStringHash(cstr s) { m_hash = StringHash(s); }
		void SetStringHash(strview sv) { m_hash = StringHash(sv); }
	};
}