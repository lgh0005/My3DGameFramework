#include "EnginePch.h"
#include "ResourceManager.h"
#include "Graphics/Geometry/GeometryGenerator.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Material.h"

bool ResourceManager::Init()
{
	// 1. 가상 경로 루트 설정
	m_virtualPaths["@Assets"] = "Assets";
	m_virtualPaths["@GameAsset"] = "Assets/Game";
	m_virtualPaths["@BuiltIn"] = "Assets/BuiltIn";

	// 2. 에셋 폴더 자동 스캔 시작 (이제 JSON 매니페스트를 일일이 적을 필요가 없습니다)
	LOG_INFO("ResourceManager: Scanning asset folders...");
	ScanFolder("@GameAsset", m_virtualPaths["@GameAsset"]);
	ScanFolder("@BuiltIn", m_virtualPaths["@BuiltIn"]);

	// 3. 빌트인 리소스 수동 생성 (GeometryGenerator 등을 이용한 런타임 생성)
	AddBuiltInResources();

	LOG_INFO("ResourceManager: Registry built with {} assets.", m_assetRegistry.size());
	return true;
}

void ResourceManager::Clear()
{
	m_resources.clear();
	LOG_INFO("ResourceManager Cleared.");
}

void ResourceManager::ScanFolder(const std::string& alias, const std::string& actualRoot)
{
	if (!fs::exists(actualRoot)) return;

	for (const auto& entry : fs::recursive_directory_iterator(actualRoot))
	{
		if (!entry.is_regular_file()) continue;

		const auto& path = entry.path();

		// 키 생성: 파일명(확장자 제외)
		std::string key = path.stem().string();

		// 가상 경로 생성
		std::string relative = fs::relative(path, actualRoot).generic_string();
		std::string virtualPath = alias + "/" + relative;

		// 레지스트리에 등록
		if (m_assetRegistry.find(key) == m_assetRegistry.end())
			m_assetRegistry[key] = virtualPath;
		else
		{
			LOG_WARN
			(
				"ResourceManager: Duplicate asset key found: '{}'. Existing: {}, New: {}",
				key, m_assetRegistry[key], virtualPath
			);
		}
	}
}

std::string ResourceManager::ResolvePath(const std::string& virtualPath) const
{
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
	// Box
	auto cubeMesh = GeometryGenerator::CreateBox();
	if (cubeMesh) AddResource<StaticMesh>(std::move(cubeMesh), "Cube", "@BuiltIn/Mesh/Cube");
	
	// Plane
	auto planeMesh = GeometryGenerator::CreatePlane();
	if (planeMesh) AddResource<StaticMesh>(std::move(planeMesh), "Plane", "@BuiltIn/Mesh/Plane");

	// Sphere (세그먼트 32 정도로 기본 생성)
	auto sphereMesh = GeometryGenerator::CreateSphere(32, 32);
	if (sphereMesh) AddResource<StaticMesh>(std::move(sphereMesh), "Sphere", "@BuiltIn/Mesh/Sphere");

	// ScreenMesh (Post-Process용 NDC Quad)
	auto screenMesh = ScreenMesh::Create();
	if (screenMesh) AddResource<ScreenMesh>(std::move(screenMesh), "Screen", "@BuiltIn/Mesh/Screen");
	
	// 기본 머티리얼
	auto defaultMat = Material::Create();
	if (defaultMat) AddResource<Material>
	(
		std::move(defaultMat), 
		defaultMat->GetName(), 
		defaultMat->GetPath()
	);
}

void ResourceManager::Clear()
{
	m_resources.clear();
	m_assetRegistry.clear();
	LOG_INFO("ResourceManager: Cleared all caches and registries.");
}