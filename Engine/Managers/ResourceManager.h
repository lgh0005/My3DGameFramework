#pragma once
#include "Resources/Resource.h"

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager)
	DECLARE_NONINSTANTIABLE(ResourceManager)

public:
	bool Init();

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

#include "Managers/ResourceManager.inl"