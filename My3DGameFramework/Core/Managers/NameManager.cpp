#include "CorePch.h"
#include "NameManager.h"

namespace MGF3D
{
	NameManager::NameManager() = default;
	NameManager::~NameManager() = default;

	StringHash NameManager::Register(const String& name)
	{
		if (name.empty()) return StringHash();

		StringHash hash(name);
		uint64 hashVal = static_cast<uint64>(hash);

		// 1. 이미 등록된 해시인지 확인
		auto it = m_nameTable.find(hashVal);
		if (it != m_nameTable.end())
		{
			MGF_ASSERT(it->second == name, "Hash collision detected in NameManager!");
			return hash;
		}

		// 2. 새로운 문자열이라면 테이블에 등록
		m_nameTable[hashVal] = name;

		return hash;
	}

	StringHash NameManager::RegisterUnique(const String& name)
	{
		if (name.empty()) return Register("NoName");

		StringHash baseHash(name);
		uint64 baseHashVal = static_cast<uint64>(baseHash);

		// 1. 만약 이 이름이 테이블에 없다면, 그냥 생으로 등록하고 카운트 시작
		if (m_nameTable.find(baseHashVal) == m_nameTable.end())
		{
			m_nameCounters[baseHashVal] = 1;
			return Register(name);
		}

		// 2. 이미 이름이 있다면, 빈 숫자를 찾을 때까지 루프
		String uniqueName = name;
		uint32& nextIndex = m_nameCounters[baseHashVal];

		while (true)
		{
			// "Name_1", "Name_2" 생성
			uniqueName = name + "_" + std::to_string(nextIndex++);
			StringHash newHash(uniqueName);
			uint64 newHashVal = static_cast<uint64>(newHash);

			// 생성한 이름이 테이블에 없으면 확정
			if (m_nameTable.find(newHashVal) == m_nameTable.end())
				return Register(uniqueName);
		}
	}

	const String& NameManager::GetString(StringHash hash) const
	{
		uint64 hashVal = static_cast<uint64>(hash);
		auto it = m_nameTable.find(hashVal);
		if (it != m_nameTable.end()) return it->second;
		return EMPTY_STRING;
	}
}