#pragma once
#include "Containers/Slab/SMap.h"
#include "Containers/Slab/SString.h"
#include "Hashing/StringHash.h"

namespace MGF3D
{
	class NameManager
	{
		DECLARE_SINGLE(NameManager)

	public:
		NameManager();
		~NameManager();

	public:
		// 문자열을 풀에 등록하고 유일한 주소를 반환
		cstr Intern(strview sv);
		cstr CreateUnique(strview baseName);
		usize GetNameCount() const { return m_stringPool.Count(); }

	private:
		SMap<StringHash, SString> m_stringPool;
		SMap<StringHash, int32> m_nameCounts;
	};
}