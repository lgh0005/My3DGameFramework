#pragma once
#include "ResourceManager.h"
#include "Misc/Enums.h"

template<typename T>
inline std::shared_ptr<T> ResourceManager::Get(const std::string& name)
{
    // 1. 인스턴스 캐시(이미 로드된 것) 확인
    auto it = m_resources.find(name);
    if (it != m_resources.end()) return TryCastResource<T>(it->second);

    // 2. 캐시에 없으면 장부(설계도) 확인
    auto regIt = m_assetRegistry.find(name);
    if (regIt != m_assetRegistry.end()) return Load<T>(*regIt->second);
    return nullptr;
}

template<typename T>
inline std::vector<std::shared_ptr<T>> ResourceManager::GetAll()
{
    std::vector<std::shared_ptr<T>> results;

    ResourceType targetType = T::GetStaticResourceType();
    for (auto& [name, resource] : m_resources)
    {
        if (auto casted = TryCastResource<T>(resource))
            results.push_back(casted);
    }

    return results;
}

template<typename T, typename... Args>
inline std::shared_ptr<T> ResourceManager::Add(const std::string& name, Args&&... args)
{
    // 1. 캐시 검색
    if (auto resource = Get<T>(name))
        return resource;

    // 2. 즉석 설계도 생성 및 로드
    typename T::DescType desc(ResolvePath(args)..., name);
    return Load<T>(desc);
}

template<typename T>
inline std::shared_ptr<T> ResourceManager::Load(const ResourceDesc& desc)
{
    std::string key = desc.GetName();

    // 1. 캐시 재확인
    auto it = m_resources.find(key);
    if (it != m_resources.end())
    {
        if (auto resource = TryCastResource<T>(it->second))
            return resource;
    }

    // 2. 실제 리소스 클래스의 정적 Load 함수 호출
    std::shared_ptr<T> resource = T::Load(static_cast<const typename T::DescType&>(desc));
    if (resource)
    {
        // 3. 실물 캐시 등록
        m_resources[key] = std::static_pointer_cast<Resource>(resource);

        // 4. 장부에도 이 설계도를 등록하여 나중에 다시 로드할 수 있게 함
        // 리소스가 멤버로 들고 있는 Desc를 장부용 shared_ptr로 래핑하여 보관합
        if (m_assetRegistry.find(key) == m_assetRegistry.end())
        {
            m_assetRegistry[key] = std::make_shared<typename T::DescType>(
                static_cast<const typename T::DescType&>(resource->GetDesc())
            );
        }
    }

    return resource;
}

template<typename T>
inline void ResourceManager::Register(const std::shared_ptr<T>& resource)
{
    if (!resource) return;

    // 1. 리소스 정보 획득
    const auto& desc = resource->GetDesc();
    const std::string& name = desc.name;
    if (name.empty())
    {
        LOG_ERROR("ResourceManager: Resource name is empty!");
        return;
    }

    // 2. 충돌 검사
    auto it = m_resources.find(name);
    if (it != m_resources.end())
    {
        std::shared_ptr<Resource> existingRes = it->second;
        if (TryCastResource<T>(existingRes) == nullptr)
        {
            LOG_ERROR
            (
                "ResourceManager: Name collision with different types! Name: [{}], Existing: [{}], New: [{}]",
                name,
                (int32)existingRes->GetResourceType(),
                (int32)T::GetStaticResourceType()
            );
            return;
        }
        LOG_WARN("ResourceManager: Overwriting existing resource [{}].", name);
    }

    // 3. 등록
    m_resources[name] = std::static_pointer_cast<Resource>(resource);
    m_assetRegistry[name] = std::make_shared<typename T::DescType>(
        static_cast<const typename T::DescType&>(desc)
    );
}

template<typename T>
inline std::shared_ptr<T> ResourceManager::TryCastResource(const std::shared_ptr<Resource>& resource)
{
    if (resource->GetResourceType() == T::GetStaticResourceType())
        return std::static_pointer_cast<T>(resource);
    return nullptr;
}