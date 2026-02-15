#include "EnginePch.h"
#include "ResourceConfigParser.h"

DECLARE_DEFAULTS_IMPL(ResourceConfigParser)

static const std::unordered_map<std::string, ResourceType> CategoryToType = 
{
	{ "GraphicShader",      ResourceType::GraphicsProgram },
	{ "ComputeShader",      ResourceType::ComputeProgram },
	{ "Texture",			ResourceType::Texture },
	{ "CubeTexture",		ResourceType::CubeTexture },
	{ "Model",				ResourceType::Model },
	{ "Animation",			ResourceType::Animation },
	{ "AudioClip",			ResourceType::AudioClip }
};

ResourceConfigParser::ExtensionMap ResourceConfigParser::ParseExtensionMap(const std::string& rootKey)
{
	m_extensionMap.clear();

	// 1. 기초 검증: 파일이 제대로 로드되었는지, 루트 키가 존재하는지 확인
	if (!m_isLoaded || !ValidateRoot(rootKey))
	{
		LOG_ERROR("ResourceConfigParser: Failed to validate root key '{}' or file not loaded.", rootKey);
		return m_extensionMap;
	}

	// 2. nlohmann::json의 items()를 사용하여 데이터 추출
	auto& extensionNode = m_jsonData[rootKey];
	if (extensionNode.is_null() || !extensionNode.is_object())
	{
		LOG_ERROR("ResourceConfigParser: '{}' node is missing or is not a JSON object.", rootKey);
		return m_extensionMap;
	}

	for (auto& [ext, typeStr] : extensionNode.items())
	{
		std::string lowerExt = ext;
		std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
		m_extensionMap[lowerExt] = typeStr.get<std::string>();
	}

	LOG_INFO
	(
		"ResourceConfigParser: Successfully parsed {} extensions from root '{}'",
		m_extensionMap.size(), rootKey
	);

	return m_extensionMap;
}

ResourceConfigParser::AssetRegistry ResourceConfigParser::ScanDirectory
(
	const std::string& alias, 
	const std::string& actualRoot
)
{
	m_assetRegistry.clear();
	if (!fs::exists(actualRoot)) return m_assetRegistry;

	for (const auto& entry : fs::recursive_directory_iterator(actualRoot))
	{
		if (!entry.is_regular_file()) continue;

		const fs::path& filePath = entry.path();
		std::string ext = filePath.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		// 1. 등록된 확장자인지 확인
		auto it = m_extensionMap.find(ext);
		if (it == m_extensionMap.end()) continue;

		// 2. 문자열 카테고리를 ResourceType 열거형으로 변환
		auto itType = CategoryToType.find(it->second);
		if (itType == CategoryToType.end()) continue;

		ResourceType resType = itType->second;
		std::string name = filePath.stem().string();
		std::string vPath = alias + "/" + fs::relative(filePath, actualRoot).generic_string();
		
		// 3. Desc 생성 및 병합
		switch (resType)
		{
			case ResourceType::GraphicsProgram:
			{
				auto& desc = m_assetRegistry[name];
				if (!desc) desc = std::make_shared<GraphicsProgramDesc>("", "", name);
				auto shaderDesc = std::static_pointer_cast<GraphicsProgramDesc>(desc);
				if (ext == ".vert") shaderDesc->vsPath = vPath;
				else if (ext == ".frag") shaderDesc->fsPath = vPath;
				break;
			}

			case ResourceType::ComputeProgram:
			{
				m_assetRegistry[name] = std::make_shared<ComputeProgramDesc>(vPath, name);
				break;
			}

			case ResourceType::Texture:
			{
				m_assetRegistry[name] = std::make_shared<TextureDesc>(vPath, name);
				break;
			}

			case ResourceType::Model:
			{
				m_assetRegistry[name] = std::make_shared<ModelDesc>(vPath, name);
				break;
			}

			case ResourceType::Animation:
			{
				m_assetRegistry[name] = std::make_shared<AnimationDesc>(vPath, name);
				break;
			}

			case ResourceType::AudioClip:
			{
				m_assetRegistry[name] = std::make_shared<AudioClipDesc>(vPath, name);
				break;
			}

			case ResourceType::CubeTexture:
			{
				m_assetRegistry[name] = std::make_shared<CubeTextureDesc>(vPath, name);
				break;
			}
		}
	}

	return m_assetRegistry;
}
