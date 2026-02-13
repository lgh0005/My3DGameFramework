#pragma once

template<typename T>
inline std::shared_ptr<T> ResourceManager::Load(const ResourceDesc& desc)
{
	// 1. 리소스 매니저 내부에서 사용할 키(Key) 생성
	// (이전 대화에서 이야기한 GetCacheKey() 가상 함수 활용)
	std::string key = desc.GetCacheKey();

	// 2. 캐시 검색 (이미 로드된 리소스가 있는지?)
	auto it = m_resources.find(key);
	if (it != m_resources.end())
	{
		// 부모(Resource) 포인터를 자식(T) 포인터로 변환하여 반환
		return std::static_pointer_cast<T>(it->second);
	}

	// 3. 없으면 새로 로드 (부모 Desc -> 자식 Desc로 캐스팅 필요)
	// T::DescType은 해당 리소스 클래스 안에 정의된 디스크립터 타입 (예: TextureDesc)
	std::shared_ptr<T> resource = T::Load(static_cast<const typename T::DescType&>(desc));

	// 4. 로드 성공 시 매니저에 등록
	if (resource)
	{
		// 맵에는 Resource 타입(부모)으로 저장됨
		m_resources[key] = resource;
	}

	return resource;
}

template<typename T>
inline void ResourceManager::AddResource(std::shared_ptr<T> resource,
	const std::string& name, const std::string& path)
{
	// 0. 리소스 유효성 체크
	if (!resource)
	{
		LOG_ERROR("Attempted to add null resource: {}", name);
		return;
	}

	// 1. 중복 체크
	if (m_resources.find(name) != m_resources.end())
	{
		LOG_WARN("Resource '{}' already exists. Overwriting.", name);
	}

	// 2. 리소스 자체에 키(이름/경로) 주입 (나중에 리소스만 보고도 뭔지 알 수 있게)
	resource->SetName(name);
	if (path.empty()) resource->SetPath("@VirtualPath/" + name);
	else			  resource->SetPath(path);

	// 3. 통합 맵에 저장 (자동 업캐스팅)
	m_resources[name] = resource;
}

template<typename T>
inline std::vector<std::shared_ptr<T>> ResourceManager::GetResources() const
{
	std::vector<std::shared_ptr<T>> result;
	result.reserve(m_resources.size());

	ResourceType targetType = T::s_ResourceType;
	for (const auto& [name, res] : m_resources)
	{
		if (res && res->MatchesType(targetType))
			result.push_back(std::static_pointer_cast<T>(res));
	}

	return result;
}

template<typename T>
inline std::shared_ptr<T> ResourceManager::GetResource(const std::string& name) const
{
	ResourceType type = T::s_ResourceType;
	auto it = m_resources.find(name);
	if (it != m_resources.end())
	{
		if (it->second->MatchesType(type))
			return std::static_pointer_cast<T>(it->second);
	}
	return nullptr;
}
