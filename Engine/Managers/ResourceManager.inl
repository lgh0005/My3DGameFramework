#pragma once
#include "ResourceManager.h"

template<typename T>
inline std::shared_ptr<T> ResourceManager::Get(const std::string& name)
{
    // 1. 캐시 검색
    auto it = m_resources.find(name);
    if (it != m_resources.end())
        return std::static_pointer_cast<T>(it->second);

    // 2. 캐시에 없으면 레지스트리(지도)에서 경로 검색
    auto regIt = m_assetRegistry.find(name);
    if (regIt == m_assetRegistry.end())
    {
        LOG_ERROR("ResourceManager: Asset '{}' not found in registry.", name);
        return nullptr;
    }

    // 3. 지연 로딩(Lazy Loading) 시작
    std::string vPath = regIt->second;
    std::string actualPath = ResolvePath(vPath);

    LOG_INFO("ResourceManager: Lazy Loading -> {} ({})", name, vPath);

    // 4. 리소스 타입에 맞는 Desc 생성 및 Load 호출
    typename T::DescType desc(actualPath, name);

    // 특수한 추가 설정이 필요한 경우 (예: Texture의 sRGB 등)는 
    // 나중에 .meta 파일이나 정책에 따라 여기에 추가할 수 있습니다.

    return Load<T>(desc);
}

template<typename T>
inline std::shared_ptr<T> ResourceManager::Load(const ResourceDesc& desc)
{
    std::string key = desc.GetCacheKey();

    // 캐시 재확인 (Load가 직접 호출될 경우 대비)
    auto it = m_resources.find(key);
    if (it != m_resources.end())
        return std::static_pointer_cast<T>(it->second);

    // 실제 리소스 클래스의 정적 Load 함수 호출
    std::shared_ptr<T> resource = T::Load(static_cast<const typename T::DescType&>(desc));
    if (resource) m_resources[key] = resource;
    return resource;
}

template<typename T>
inline void ResourceManager::AddResource(std::shared_ptr<T> resource,
    const std::string& name, const std::string& path)
{
    if (!resource) return;

    resource->SetName(name);
    resource->SetPath(path.empty() ? "@Virtual/" + name : path);

    m_resources[name] = resource;
}