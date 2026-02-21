#pragma once
#include "Containers/Slab/SMap.h"
#include "Containers/Slab/SString.h"
#include "Hashing/StringHash.h"

namespace MGF3D
{
	struct MGFName;

	class NameManager
	{
		DECLARE_SINGLE(NameManager)

	public:
		NameManager();
		~NameManager();

	public:
		MGFName AddName(cstr name);
		MGFName AddName(strview name);
		usize GetNameCount() const { return m_stringPool.Count(); }

	private:
		MGFName CreateUniqueName(strview name);
		MGFName RegisterNewName(strview name, const StringHash& hash);

	private:
		SMap<StringHash, SString> m_stringPool;
		SMap<StringHash, int32> m_nameCounts;
	};
}