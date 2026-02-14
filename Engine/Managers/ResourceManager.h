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

	template<typename T, typename... Args> 
	std::shared_ptr<T> Get(const std::string& name, Args&&... args);
	template<typename T> std::shared_ptr<T> Load(const ResourceDesc& desc);
	template<typename T> void AddResource
	(
		std::shared_ptr<T> resource, 
		const std::string& name, 
		const std::string& path
	);

private:
	void ScanFolder(const std::string& alias, const std::string& actualRoot);
	std::string ResolvePath(const std::string& virtualPath) const;
	void AddBuiltInResources();

private:
	std::unordered_map<std::string, std::shared_ptr<Resource>> m_resources;
	std::unordered_map<std::string, std::string> m_assetRegistry;
	std::unordered_map<std::string, std::string> m_virtualPaths;
};

#include "Managers/ResourceManager.inl"