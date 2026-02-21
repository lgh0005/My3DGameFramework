#include "CorePch.h"
#include "NameManager.h"
#include "Containers/Linear/LString.h"
#include "Utils/StringUtils.h"
#include "Utils/CoreUtils.h"
#include "Naming/MGFName.h"

namespace MGF3D
{
    NameManager::NameManager() = default;
    NameManager::~NameManager() = default;

    MGFName NameManager::AddName(cstr name, bool makeUnique)
    {
        return AddName(strview(name), makeUnique);
    }

    MGFName NameManager::AddName(strview name, bool makeUnique)
    {
        if (name.empty()) return MGFName();

        // 해당 문자열 해시 생성
        StringHash hash(name);

        // 중복 체크
        if (m_stringPool.Find(hash))
        {
            // 1. 고유 옵션이 켜져 있으면 _1, _2를 생성
            if (makeUnique)
                return CreateUniqueName(name);

            // 2. 그렇지 않으면 장부에 있는 똑같은(풀링된) 주소를 그대로
            MGFName result;
            result.SetString(m_stringPool.Find(hash)->CStr());
            result.SetStringHash(hash);
            return result;
        }

        return RegisterNewName(name, hash);
    }

    MGFName NameManager::CreateUniqueName(strview baseName)
    {
        StringHash baseHash(baseName);

        // 이 기반 이름이 몇 번이나 요청되었는지 장부(m_nameCounts) 확인
        int32* pCount = m_nameCounts.Find(baseHash);
        int32 currentCount = 0;

        if (pCount == nullptr)
        {
            currentCount = 1;
            m_nameCounts.Insert(baseHash, currentCount);
        }
        else
        {
            (*pCount)++;
            currentCount = *pCount;
        }

        // 새로운 이름 및 그 이름의 해시 생성
        LString uniqueName = LString(baseName) + "_" + StringUtils::ToString(currentCount);
        StringHash newHash(uniqueName);

        return RegisterNewName(uniqueName, newHash);
    }

    MGFName NameManager::RegisterNewName(strview name, const StringHash& hash)
    {
        // Slab 영역의 영구 문자열 풀에 데이터 삽입
        m_stringPool.Insert(hash, SString(name));

        // 최종적인 이름
        MGFName result;

        // 셋터를 통해 풀링된 '진짜' 주소와 해시를 직접 주입.
        auto* pooledSString = m_stringPool.Find(hash);
        result.SetString(pooledSString->CStr());
        result.SetStringHash(hash);

        return result;
    }
}