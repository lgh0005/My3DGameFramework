#include "CorePch.h"
#include "NameManager.h"
#include "Containers/Linear/LString.h"
#include "Utils/StringUtils.h"

namespace MGF3D
{
    NameManager::NameManager() = default;
    NameManager::~NameManager() = default;

    cstr NameManager::Intern(strview sv)
	{
        if (sv.empty()) return "";

        StringHash hash(sv);

        // 이미 존재하는지 확인
        if (auto* existing = m_stringPool.Find(hash))
            return existing->CStr();

        // 새로운 문자열 등록
        SString newStr(sv);
        cstr internalPtr = newStr.CStr();
        m_stringPool.Insert(hash, std::move(newStr));

        return internalPtr;
	}

	cstr NameManager::CreateUnique(strview baseName)
	{
        StringHash baseHash(baseName);

        // 기존에 이 이름으로 요청된 적이 있는지 확인
        int32* pCount = m_nameCounts.Find(baseHash);
        if (pCount == nullptr)
        {
            // 처음이라면 카운트 0으로 시작하고 원본 이름 반환
            m_nameCounts.Insert(baseHash, 0);
            return Intern(baseName);
        }

        // 중복이라면 카운트 증가 후 새로운 문자열 가공
        (*pCount)++;
        LString uniqueName = LString(baseName) + "_" + StringUtils::ToString(*pCount);

        return Intern(uniqueName);
	}
}