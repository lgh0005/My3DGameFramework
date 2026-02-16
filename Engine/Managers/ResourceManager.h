#pragma once
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"
#include "Misc/ResourceFormat.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Resource)
CLASS_PTR(ResourceDesc)
#pragma endregion

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager)
	DECLARE_NONINSTANTIABLE(ResourceManager)

public:
	bool Init();
	void Clear();
	std::string ResolvePath(const std::string& virtualPath) const;

	template<typename T> 
	std::shared_ptr<T> Get(const std::string& name);

	template<typename T>
	std::vector<std::shared_ptr<T>> GetAll();

	template<typename T, typename... Args> 
	std::shared_ptr<T> Add(const std::string& name, Args&&... args);

	template<typename T>
	void Register(const std::shared_ptr<T>& resource);

private:
	template<typename T>
	std::shared_ptr<T> Load(const ResourceDesc& desc);

	template<typename T>
	std::shared_ptr<T> TryCastResource(const std::shared_ptr<Resource>& resource);

	bool LoadEngineConfig(const std::string& path);
	void AddBuiltInResources();

private:
	std::unordered_map<std::string, ResourcePtr>     m_engineResources; // { name, @BuiltIn }
	std::unordered_map<std::string, ResourcePtr>     m_resources;       // { name, @GameAssets, @Virtauals }
	std::unordered_map<std::string, ResourceDescPtr> m_assetRegistry;   // { name, resource_desc }
	std::unordered_map<std::string, std::string>     m_virtualPaths;    // { virtual_path, real_path }
};

#include "Managers/ResourceManager.inl"