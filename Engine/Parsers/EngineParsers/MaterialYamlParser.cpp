#include "EnginePch.h"
#include "MaterialYamlParser.h"
#include "Misc/Constants.h"
#include "Resources/Material.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Textures/TextureUtils.h"
#include "Misc/Utils.h"
#include <nlohmann/json.hpp>

DECLARE_DEFAULTS_IMPL(MaterialYamlParser)

bool MaterialYamlParser::LoadConfig(const std::string& configPath)
{
	std::string fullPath = ResolvePath(configPath);
	std::ifstream file(fullPath);
	if (!file.is_open())
	{
		m_lastErrorMessage = "Failed to open Material Config: " + fullPath;
		LOG_ERROR(m_lastErrorMessage);
		return false;
	}

	nlohmann::json config = nlohmann::json::parse(file);
	if (config.contains(Constants::KEY_TOKENS))
	{
		for (auto& [key, value] : config[Constants::KEY_TOKENS].items())
		{
			uint32 hash = Utils::StrHash(key);
			m_tokenMap[hash] = value.get<std::string>();
		}
	}

	LOG_INFO("Material Config Loaded. Tokens: {}", m_tokenMap.size());
	return true;
}

bool MaterialYamlParser::LoadMaterial(Material* outMaterial)
{
	// 1. 기본 유효성 검사
	if (!IsLoaded() || !m_rootNode.IsDefined())
	{
		LOG_ERROR("YAML not loaded. Call LoadFromFile first.");
		return false;
	}
	if (!outMaterial) return false;

	// 2. 루트 키 확인 ("Material")
	std::string rootKey(Constants::ROOT_MATERIAL);
	if (!ValidateRoot(rootKey)) return false;

	// 3. 재질 노드 가져오기
	YAML::Node matNode = m_rootNode[rootKey];

	// 4. 텍스처 로드 (내부 헬퍼 사용)
	outMaterial->diffuse = LoadTextureInternal(Constants::MatTokens::TEX_DIFFUSE);
	outMaterial->normal = LoadTextureInternal(Constants::MatTokens::TEX_NORMAL);
	outMaterial->specular = LoadTextureInternal(Constants::MatTokens::TEX_SPECULAR);
	outMaterial->emission = LoadTextureInternal(Constants::MatTokens::TEX_EMISSION);
	outMaterial->height = LoadTextureInternal(Constants::MatTokens::TEX_HEIGHT);
	outMaterial->ao = LoadTextureInternal(Constants::MatTokens::TEX_AO);
	outMaterial->metallic = LoadTextureInternal(Constants::MatTokens::TEX_METALLIC);
	outMaterial->roughness = LoadTextureInternal(Constants::MatTokens::TEX_ROUGHNESS);
	outMaterial->orm = LoadTextureInternal(Constants::MatTokens::TEX_ORM);

	// 5. 속성 값 로드
	outMaterial->shininess = LoadFloat(matNode, Constants::MatTokens::VAL_SHININESS, outMaterial->shininess);
	outMaterial->emissionStrength = LoadFloat(matNode, Constants::MatTokens::VAL_EMISSION_STRENGTH, outMaterial->emissionStrength);
	outMaterial->heightScale = LoadFloat(matNode, Constants::MatTokens::VAL_HEIGHT_SCALE, outMaterial->heightScale);
	outMaterial->metallicFactor = LoadFloat(matNode, Constants::MatTokens::VAL_METALLIC, outMaterial->metallicFactor);
	outMaterial->roughnessFactor = LoadFloat(matNode, Constants::MatTokens::VAL_ROUGHNESS, outMaterial->roughnessFactor);
	outMaterial->albedoFactor = LoadVec4(matNode, Constants::MatTokens::VAL_ALBEDO_COLOR, outMaterial->albedoFactor);
	outMaterial->emissiveFactor = LoadVec3(matNode, Constants::MatTokens::VAL_EMISSIVE_COLOR, outMaterial->emissiveFactor);
	outMaterial->useGlossinessAsRoughness = LoadBool(matNode, Constants::MatTokens::FLAG_USE_GLOSSINESS, outMaterial->useGlossinessAsRoughness);

	return true;
}

TexturePtr MaterialYamlParser::LoadTextureInternal(std::string_view tokenKey)
{
	// 1. 토큰 맵에서 실제 YAML 키 찾기
	std::string yamlKey = GetToken(Utils::StrHash(tokenKey.data()));
	if (yamlKey.empty()) return nullptr;

	// 2. Material 노드 아래에서 파일명 찾기
	YAML::Node matNode = m_rootNode[std::string(Constants::ROOT_MATERIAL)];

	// 파일명이 존재하는지 확인
	if (matNode[yamlKey] && !matNode[yamlKey].IsNull())
	{
		std::string filename = matNode[yamlKey].as<std::string>();
		if (filename.empty()) return nullptr;

		// 3. 경로 조립: YAML 파일 위치 기준 상대 경로
		// 예: Assets/Models/Knight/Knight.yaml -> Assets/Models/Knight/Albedo.png
		fs::path yamlPath(m_filePath);
		fs::path texPath = yamlPath.parent_path() / filename;

		// 4. 리소스 매니저 로드
		// 
		// return RESOURCE.GetResource<Texture>(texPath.string());
	}

	return nullptr;
}
