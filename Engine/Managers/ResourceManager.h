#pragma once
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"
#include "Misc/ResourceFormat.h"

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager)
	DECLARE_NONINSTANTIABLE(ResourceManager)

public:
	bool Init();
	void Clear();

	template<typename T> std::shared_ptr<T> Load(const ResourceDesc& desc);
	template<typename T> std::shared_ptr<T> GetResource(const std::string& name) const;
	template<typename T> std::vector<std::shared_ptr<T>> GetResources() const;
	template<typename T>
	void AddResource
	(
		std::shared_ptr<T> resource,
		const std::string& name,
		const std::string& path = ""
	);

private:


private:
	std::unordered_map<std::string, std::shared_ptr<Resource>> m_resources;
};

#include "Managers/ResourceManager.inl"