#pragma once
#include "Resources/Resource.h"

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager)

public:
	template<typename T>
	std::shared_ptr<T> GetResource(const std::string& name) const;

	template<typename T>
	void AddResource
	(
		std::shared_ptr<T> resource,
		const std::string& name,
		const std::string& path = ""
	);

	void Clear();

private:
	std::unordered_map<std::string, std::shared_ptr<Resource>> m_resources;
};

/*=======================================//
//   resource manager template inlines   //
//=======================================*/
#pragma region TEMPLATE_IMPLEMENTATIONS

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

#pragma endregion