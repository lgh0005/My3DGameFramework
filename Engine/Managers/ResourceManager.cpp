#include "EnginePch.h"
#include "ResourceManager.h"
#include "Graphics/Geometry/GeometryGenerator.h"
#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Material.h"
#include "Parsers/JsonParser.h"
#include "Resources/Textures/Texture.h"

// TODO : 몇 가지 하드코딩된 문자열은 이후에 문자열 상수들로 따로 빼야 함

bool ResourceManager::Init()
{
	// 0. 부트스트랩: 가상 경로 시스템을 가동하기 위한 최소한의 경로를 수동 등록
	m_virtualPaths["@Config"] = "Config";

	// 1. 엔진 부트 설정(가상 경로) 로드
	if (!LoadEngineConfig(ResolvePath("@Config/EngineConfig.json")))
	{
		LOG_ERROR("ResourceManager: Failed to load EngineConfig.json");
		return false;
	}

	// 2. ResourceConfigParser를 이용한 에셋 설계도 스캔
	// TODO : 이쪽도 약간 생각을 해볼 필요는 있음
	ResourceConfigParser configParser;
	if (configParser.LoadFromFile(ResolvePath("@Config/ResourceConfig.json")))
	{
		configParser.ParseExtensionMap("ResourceExtensions");

		// 각 루트 폴더를 스캔하여 Desc 정보를 획득하고 장부에 등록
		auto gameAssets = configParser.ScanDirectory("@GameAsset", ResolvePath("@GameAsset"));
		auto builtInAssets = configParser.ScanDirectory("@BuiltInAsset", ResolvePath("@BuiltInAsset"));
		m_assetRegistry.insert(gameAssets.begin(), gameAssets.end());
		m_assetRegistry.insert(builtInAssets.begin(), builtInAssets.end());
	}
	else
	{
		LOG_ERROR("ResourceManager: Failed to load ResourceConfig.json");
		return false;
	}

	// 3. 빌트인 리소스 수동 생성
	AddBuiltInResources();

	LOG_INFO("ResourceManager: Registry built with {} asset blueprints.", m_assetRegistry.size());
	return true;
}

void ResourceManager::Clear()
{
	m_resources.clear();
	m_assetRegistry.clear();
	LOG_INFO("ResourceManager Cleared.");
}

bool ResourceManager::LoadEngineConfig(const std::string& path)
{
	// 1. 스택에 파서 생성 및 로드
	JsonParser parser;
	if (!parser.LoadFromJsonFile(path))
	{
		LOG_ERROR("ResourceManager: Failed to load EngineConfig at {}", path);
		return false;
	}

	// 2. 가상 경로 섹션 검증
	if (!parser.HasTypeOf<nlohmann::json::object_t>("virtualPaths"))
	{
		LOG_ERROR("ResourceManager: 'virtualPaths' is missing or invalid in {}", path);
		return false;
	}

	// 3. 실제 데이터 매핑
	const auto& root = parser.GetRoot();
	const auto& pathsNode = root["virtualPaths"];
	for (auto& [alias, actualPath] : pathsNode.items())
	{
		if (actualPath.is_string())
			m_virtualPaths[alias] = actualPath.get<std::string>();
	}

	LOG_INFO("ResourceManager: Successfully mapped {} virtual paths.", m_virtualPaths.size());
	return true;
}

std::string ResourceManager::ResolvePath(const std::string& virtualPath) const
{
	// 0. 비어있거나 @로 시작하지 않으면 가상 경로가 아님
	if (virtualPath.empty() || virtualPath[0] != '@') 
		return virtualPath;

	// 1. fs::path로 변환 및 가상 경로 토큰 획득
	fs::path p(virtualPath);
	auto it = p.begin();
	std::string alias = it->string();

	// 2. 가상 경로 매핑 확인
	auto mapIt = m_virtualPaths.find(alias);
	if (mapIt == m_virtualPaths.end()) return virtualPath;

	// 3. 매핑된 실제 경로로 시작하는 결과 경로 생성
	fs::path resolvedPath = mapIt->second;

	// 4. 나머지 조각들을 순차적으로 결합 (하부 경로 조립)
	for (++it; it != p.end(); ++it)
		resolvedPath /= *it;

	// 7. 정문화 및 반환
	return resolvedPath.lexically_normal().string();
}

// [TEMP]
void ResourceManager::AddBuiltInResources()
{
	// [도우미 람다] 생성된 리소스에 정체성을 부여하고 등록합니다.
	auto QuickRegister = [&](const std::string& name, const std::string& vPath, auto resource)
	{
		if (!resource) return;
		resource->GetDesc().name = name;
		resource->GetDesc().path = vPath;
		Register(std::move(resource));
	};

	// 1. 기본 메쉬 (GeometryGenerator 활용)
	QuickRegister("Cube", "@BuiltIn/Mesh/Cube", GeometryGenerator::CreateBox());
	QuickRegister("Plane", "@BuiltIn/Mesh/Plane", GeometryGenerator::CreatePlane());
	QuickRegister("Sphere", "@BuiltIn/Mesh/Sphere", GeometryGenerator::CreateSphere(32, 32));

	// 2. 특수 메쉬
	QuickRegister("Screen", "@BuiltIn/Mesh/Screen", ScreenMesh::Create());

	// 3. 기본 머티리얼
	QuickRegister("DefaultMaterial", "@BuiltIn/Material", Material::Create());

	// [TEMP] 카메라 흙 추가
	TextureDesc dirtDesc("@GameAsset/Images/baked/camera_dirt.ktx", "camera_dirt");
	dirtDesc.sRGB = false;
	auto dirtTex = Texture::Load(dirtDesc);
	if (dirtTex) QuickRegister("camera_dirt", dirtDesc.path, std::move(dirtTex));
}
