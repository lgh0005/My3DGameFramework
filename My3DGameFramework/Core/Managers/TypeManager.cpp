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

    Ptr<const MGFType> TypeManager::GetType(StringHash id) const
    {
        // const Find를 사용하여 안전하게 조회
        auto found = m_tokenRegistry.Find(id);
        if (found) return (*found).Get();
        return nullptr;
    }

    void TypeManager::BuildHierarchy()
    {
        if (m_typeTokenList.Empty()) return;

        // 1단계: 모든 타입 객체를 메모리에 생성
        InitTypeObjects();

        // 2단계: 부모-자식 관계 및 족보(Chain) 완성
        ResolveTypeHierarchy();

        m_typeTokenList.Clear();
    }

    void TypeManager::InitTypeObjects()
    {
        for (const auto& typeToken : m_typeTokenList)
        {
            StringHash id = typeToken.name.GetStringHash();
            if (m_tokenRegistry.Find(id) != nullptr) continue;

            auto newType = MakeUnique<MGFType>();
            newType->name = typeToken.name;
            newType->id = id;

            m_tokenRegistry.Insert(id, std::move(newType));
        }
    }

    void TypeManager::ResolveTypeHierarchy()
    {
        bool bChanged = true;
        uint32 passCount = 0;
        const uint32 maxPasses = MGFType::MAX_DEPTH + 1;

        while (bChanged && passCount < maxPasses)
        {
            bChanged = false;
            passCount++;

            for (const auto& token : m_typeTokenList)
            {
                if (ResolveSpecificType(token))
                    bChanged = true;
            }
        }
    }

    bool TypeManager::ResolveSpecificType(const TypeToken& token)
    {
        StringHash id = token.name.GetStringHash();
        auto foundTarget = m_tokenRegistry.Find(id);
        if (!foundTarget) return false;

        Ptr<MGFType> target = (*foundTarget).Get();

        // 1. 이미 해결된 타입이면 패스
        if (target->depth > 0 || (target->chain[0].IsValid())) return false;

        // 2. 루트 클래스 처리
        if (token.parentName.IsEmpty())
        {
            target->parent = nullptr;
            target->depth = 0;
            target->chain[0] = target->id;
            return true;
        }

        // 3. 자식 클래스 처리
        StringHash parentID = token.parentName.GetStringHash();
        auto foundParent = m_tokenRegistry.Find(parentID);
        if (foundParent)
        {
            Ptr<MGFType> parent = (*foundParent).Get();
            bool isParentReady = (parent->depth > 0 || parent->chain[0].IsValid());

            if (isParentReady)
            {
                uint32 newDepth = parent->depth + 1;
                if (newDepth >= MGFType::MAX_DEPTH)
                {
                    MGF_LOG_ERROR("Type hierarchy too deep for: {}", token.name.CStr());
                    return false;
                }

                target->parent = parent;
                target->depth = newDepth;

                // 족보 복사
                for (uint32 i = 0; i <= parent->depth; ++i)
                    target->chain[i] = parent->chain[i];

                target->chain[target->depth] = target->id;
                return true;
            }
        }

        return false;
    }
}