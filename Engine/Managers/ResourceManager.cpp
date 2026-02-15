#include "EnginePch.h"
#include "ResourceManager.h"
#include "Graphics/Geometry/GeometryGenerator.h"
#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Material.h"
#include "Parsers/JsonParser.h"
#include "Parsers/EngineParsers/ResourceConfigParser.h"

#include "Resources/Textures/Texture.h"

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
	// 0. 기본 JsonParser를 사용하여 EngineConfig.json을 읽습니다.
	JsonParser parser;
	if (!parser.LoadFromFile(path))
	{
		LOG_ERROR("ResourceManager: Failed to load EngineConfig at {}", path);
		return false;
	}

	// 1. virtualPaths 섹션 유효성 검사
	if (!parser.ValidateRoot("virtualPaths")) return false;

	// 2. 가상 경로 매핑 시작
	auto& pathsNode = parser.GetJsonData()["virtualPaths"];
	if (pathsNode.is_null() || !pathsNode.is_object())
	{
		LOG_ERROR("ResourceManager: 'virtualPaths' is not a valid JSON object.");
		return false;
	}

	for (auto& [alias, actualPath] : pathsNode.items())
		m_virtualPaths[alias] = actualPath.get<std::string>();

	return true;
}

std::string ResourceManager::ResolvePath(const std::string& virtualPath) const
{
	// TODO : 이거 리졸빙 로직을 확실히 해야함.
	// 처음 등장한 슬래시 기준, 읽어들인 문자열이 @로 시작하며, 오로지 @만
	// 있는 경우는 제외해야 한다.
	if (virtualPath.empty() || virtualPath[0] != '@') return virtualPath;

	usize slashPos = virtualPath.find('/');
	std::string alias = virtualPath.substr(0, slashPos);
	std::string subPath = (slashPos != std::string::npos) ? virtualPath.substr(slashPos) : "";

	auto it = m_virtualPaths.find(alias);
	if (it != m_virtualPaths.end()) return it->second + subPath;
	return virtualPath;
}

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
