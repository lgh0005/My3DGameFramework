#include "CorePch.h"
#include "TypeManager.h"

namespace MGF3D
{
	TypeManager::TypeManager() = default;
	TypeManager::~TypeManager() = default;

    void TypeManager::TagType(const MGFName& name, const MGFName& parentName)
    {
        m_typeTokenList.PushBack({ name, parentName });
    }

    void TypeManager::BuildHierarchy()
    {
        if (m_typeTokenList.Empty()) return;

        // 1. 모든 타입 객체(MGFType)부터 먼저 생성 (UniquePtr로 안전하게 소유)
        for (const auto& typeToken : m_typeTokenList)
        {
            StringHash id = typeToken.name.GetStringHash();

            // 이미 등록된 타입이면 건너뜀
            if (m_tokenRegistry.Find(id) != nullptr) continue;

            auto newType = std::make_unique<MGFType>();
            newType->name = typeToken.name;
            newType->id = id;

            m_tokenRegistry.Insert(id, std::move(newType));
        }

        // 2. 족보(Chain) 계산 로직 (의존성 해결 루프)
        bool bChanged = true;
        while (bChanged)
        {
            bChanged = false;

            for (const auto& token : m_typeTokenList)
            {
                StringHash id = token.name.GetStringHash();

                // Find를 통해 UniquePtr의 주소를 가져옴
                auto foundTarget = m_tokenRegistry.Find(id);
                if (!foundTarget) continue;

                MGFType* target = (*foundTarget).get();

                // 이미 족보가 완성되었거나 루트 클래스인 경우 처리
                if (target->depth > 0 || token.parentName.IsEmpty())
                {
                    if (token.parentName.IsEmpty() && target->chain[0] == 0)
                    {
                        target->chain[0] = target->id;
                        target->depth = 0;
                        target->parent = nullptr;
                        bChanged = true;
                    }
                    continue;
                }

                // 부모 타입 정보 찾기
                StringHash parentID = token.parentName.GetStringHash();
                auto foundParent = m_tokenRegistry.Find(parentID);

                if (foundParent)
                {
                    MGFType* parent = (*foundParent).get();

                    // 부모의 족보가 이미 완성되어 있어야 내 족보를 완성할 수 있음
                    bool isParentReady = (parent->depth > 0 || (parent->parent == nullptr && parent->chain[0] != 0));

                    if (isParentReady)
                    {
                        target->parent = parent;
                        target->depth = parent->depth + 1;

                        if (target->depth < MGFType::MAX_DEPTH)
                        {
                            for (uint32 i = 0; i <= parent->depth; ++i)
                                target->chain[i] = parent->chain[i];

                            target->chain[target->depth] = target->id;
                            bChanged = true;
                        }
                    }
                }
            }
        }

        m_typeTokenList.Clear();
    }

    Ptr<const MGFType> TypeManager::GetType(StringHash id) const
    {
        // const Find를 사용하여 안전하게 조회
        auto found = m_tokenRegistry.Find(id);
        if (found) return (*found).get();
        return nullptr;
    }

}