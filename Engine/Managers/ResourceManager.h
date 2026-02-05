#pragma once
#include "Resources/Resource.h"
#include "Misc/ResourceFormat.h"

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager)
	DECLARE_NONINSTANTIABLE(ResourceManager)

public:
	bool Init();

	template<typename T> std::shared_ptr<T> GetResource(const std::string& name) const;
	template<typename T> std::vector<std::shared_ptr<T>> GetResources() const;

	template<typename T>
	void AddResource
	(
		std::shared_ptr<T> resource,
		const std::string& name,
		const std::string& path = ""
	);

	void Clear();

/*=======================================//
//   Data-driven resource loading test   //
//=======================================*/
public:
	void LoadAssetConfig(); // AssetConfig.json 로드
	void LoadAllInDirectory(const std::string& virtualPath);
	void ImportResource(const fs::path& filePath); // 개별 파일 로드 분기 처리

private:
	std::unordered_map<std::string, std::shared_ptr<Resource>> m_resources;
	std::unordered_map<std::string, AssetType> m_extToTypeMap;
};

#include "Managers/ResourceManager.inl"