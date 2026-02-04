#pragma once
#include "Misc/ResourceJsonFormat.h"
using namespace ResourceJsonFmt;

CLASS_PTR(ResourceLoader)
class ResourceLoader
{
	DECLARE_STATIC_CLASS(ResourceLoader)

public:
	static bool Init();
	static bool Load(const std::string& manifestPath);

/*==============================//
//   resource loading methods   //
//==============================*/
private:
	static bool LoadResourcesFromManifest(const ResourceManifest& manifest);
	static bool LoadBuiltInResources();
	static bool LoadShaders(const std::vector<ShaderData>& dataList);
	static bool LoadComputeShaders(const std::vector<ResourceData>& dataList);
	static bool LoadTextures(const std::vector<ResourceData>& dataList);
	static bool LoadCubeTextures(const std::vector<CubeMapData>& dataList);
	static bool LoadModels(const std::vector<ResourceData>& dataList);
	static bool LoadAnimations(const std::vector<ResourceData>& dataList);
	static bool LoadBGM(const std::vector<ResourceData>& dataList);
	static bool LoadSFX(const std::vector<ResourceData>& dataList);

/*===================================//
//   json menifest parsing methods   //
//===================================*/
private:
	// 메인 파싱 함수
	static std::optional<ResourceManifest> ParseManifest(const std::string& manifestPath);

	// [수정] 아래 파싱 헬퍼 함수들에 'defaultRoot' 인자가 추가되었습니다.

	static void ParseGeneralResources
	(
		const nlohmann::json& jsonContext,
		const std::string& categoryKey,
		const std::string& defaultRoot, // <--- 추가됨
		std::vector<ResourceData>& outList
	);

	static void ParseShaderResources
	(
		const nlohmann::json& jsonContext,
		const std::string& categoryKey,
		const std::string& defaultRoot, // <--- 추가됨
		std::vector<ShaderData>& outList
	);

	static void ParseCubeMapResources
	(
		const nlohmann::json& jsonContext,
		const std::string& categoryKey,
		const std::string& defaultRoot, // <--- 추가됨
		std::vector<CubeMapData>& outList
	);
};