#pragma once
#include "Containers/Slab/SMap.h"
#include "Containers/Slab/SString.h"
#include "Hashing/StringHash.h"

// TODO : 아마 여기에서도 락이 필요할 수 있음

namespace MGF3D
{
	struct MGFName;

	class NameManager
	{
		MGF_DECLARE_SINGLE(NameManager)

	private:
		NameManager();
		~NameManager();

	public:
		void Shutdown();
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