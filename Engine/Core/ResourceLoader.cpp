#include "EnginePch.h"
#include "ResourceLoader.h"
#include "Managers/FileManager.h" // [필수] 파일 매니저 포함

#include "Graphics/Geometry/GeometryGenerator.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"
#include "Resources/Programs/ComputeProgram.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Model.h"
#include "Resources/Animations/Animation.h"
#include "Resources/Font.h"
#include "Resources/Image.h"
#include "Resources/AudioClip.h"
#include "Resources/Textures/CubeTexture.h"
#include "Resources/Textures/TextureUtils.h"
#include "Resources/Meshes/StaticMesh.h"
#include "Resources/Material.h"

/*======================//
//  main entry method   //
//======================*/
bool ResourceLoader::Init()
{
	LOG_INFO("Initializing Built-in Resources...");
	return LoadBuiltInResources();
}

bool ResourceLoader::Load(const std::string& manifestPath)
{
	// manifestPath는 이미 ResourceManager에서 Resolve된 절대 경로로 들어옵니다.
	LOG_INFO("Start Resource Loading... : {}", manifestPath);
	bool success = true;

	// 1. JSON 매니페스트 파싱 (여기서 가상 경로 조립)
	auto manifestOpt = ParseManifest(manifestPath);

	// 2. 파싱 성공 시 외부 리소스 로드 진행 (여기서 Resolve 및 로드)
	if (manifestOpt.has_value())
	{
		if (!LoadResourcesFromManifest(manifestOpt.value()))
			success = false;
	}
	else
	{
		LOG_ERROR("ResourceLoader: Failed to parse manifest '{}'.", manifestPath);
		success = false;
	}

	return success;
}

/*==============================//
//   resource loading methods   //
//==============================*/
bool ResourceLoader::LoadResourcesFromManifest(const ResourceManifest& manifest)
{
	// 각 Load 함수 내부에서 FILE_MGR.Resolve()를 호출합니다.

	if (!LoadShaders(manifest.shaders)) return false;
	if (!LoadComputeShaders(manifest.computeShaders)) return false;
	if (!LoadTextures(manifest.textures)) return false;
	if (!LoadCubeTextures(manifest.cubemaps)) return false;
	if (!LoadModels(manifest.models)) return false;
	if (!LoadAnimations(manifest.animations)) return false;
	if (!LoadBGM(manifest.bgm)) return false;
	if (!LoadSFX(manifest.sfx)) return false;

	return true;
}

// BuiltIn은 메모리 생성이라 경로가 필요 없음 (변경 없음)
bool ResourceLoader::LoadBuiltInResources()
{
	StaticMeshPtr mesh;

	// 1. Cube
	mesh = GeometryGenerator::CreateBox();
	if (!mesh) return false;
	RESOURCE.AddResource<StaticMesh>(mesh, "Cube", "@BuiltIn/Cube");

	// 2. Plane
	mesh = GeometryGenerator::CreatePlane();
	if (!mesh) return false;
	RESOURCE.AddResource<StaticMesh>(mesh, "Plane", "@BuiltIn/Plane");

	// 3. Sphere
	mesh = GeometryGenerator::CreateSphere();
	if (!mesh) return false;
	RESOURCE.AddResource<StaticMesh>(mesh, "Sphere", "@BuiltIn/Sphere");

	// 4. ScreenMesh
	ScreenMeshPtr scrnMesh;
	scrnMesh = ScreenMesh::Create();
	if (!scrnMesh) return false;
	RESOURCE.AddResource<ScreenMesh>(scrnMesh, "Screen", "@BuiltIn/Screen");

	// 5. Default Materials
	MaterialPtr material;

	material = Material::Create();
	if (!material) return false;
	RESOURCE.AddResource<Material>(material, "material_SRP", "@BuiltIn/Material/SRP");

	material = Material::Create();
	if (!material) return false;
	RESOURCE.AddResource<Material>(material, "material_URP", "@BuiltIn/Material/URP");

	return true;
}

// ---------------------------------------------------------
// [핵심 변경] 모든 Load 함수에 Resolve 적용
// ---------------------------------------------------------

bool ResourceLoader::LoadShaders(const std::vector<ShaderData>& dataList)
{
	bool success = true;
	for (const auto& data : dataList)
	{
		// VS, FS 경로 각각 해석
		std::string vsAbsPath = FILE_MGR.Resolve(data.vsPath);
		std::string fsAbsPath = FILE_MGR.Resolve(data.fsPath);

		GraphicsProgramUPtr shader = GraphicsProgram::Create(vsAbsPath, fsAbsPath);
		if (shader)
			RESOURCE.AddResource<GraphicsProgram>(std::move(shader), data.key);
		else
		{
			LOG_ERROR("Failed to create Shader: '{}'", data.key);
			success = false;
		}
	}
	return success;
}

bool ResourceLoader::LoadComputeShaders(const std::vector<ResourceData>& dataList)
{
	bool success = true;
	for (const auto& data : dataList)
	{
		std::string absPath = FILE_MGR.Resolve(data.path);
		ComputeProgramUPtr shader = ComputeProgram::Create(absPath);

		if (shader)
			RESOURCE.AddResource<ComputeProgram>(std::move(shader), data.key);
		else
		{
			LOG_ERROR("Failed to create ComputeProgram: '{}' ({})", data.key, absPath);
			success = false;
		}
	}
	return success;
}

bool ResourceLoader::LoadTextures(const std::vector<ResourceData>& dataList)
{
	bool success = true;
	for (const auto& data : dataList)
	{
		// 가상 경로(@Game/...)를 절대 경로(C:/.../Bin/...)로 변환
		std::string absPath = FILE_MGR.Resolve(data.path);

		auto texture = TextureUtils::LoadTexture(absPath);
		if (texture)
			RESOURCE.AddResource<Texture>(std::move(texture), data.key, data.path); // path는 디버깅용으로 원본(가상) 유지 추천
		else
		{
			LOG_ERROR("Failed to create Texture: '{}' ({})", data.key, absPath);
			success = false;
		}
	}
	return success;
}

bool ResourceLoader::LoadCubeTextures(const std::vector<CubeMapData>& dataList)
{
	bool success = true;
	for (const auto& data : dataList)
	{
		CubeTextureUPtr texture = nullptr;
		usize fileCount = data.paths.size();

		// 모든 경로를 미리 Resolve
		std::vector<std::string> absPaths;
		absPaths.reserve(fileCount);
		for (const auto& virtualPath : data.paths)
		{
			absPaths.push_back(FILE_MGR.Resolve(virtualPath));
		}

		switch (fileCount)
		{
		case 1: // .ktx, .hdr 등 단일 파일
		{
			std::string path = absPaths[0];
			std::string ext = fs::path(path).extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			if (ext == ".ktx") texture = TextureUtils::LoadCubeMapFromKtx(path);
			// 추후 .hdr 등 지원 추가 가능
			break;
		}
		case 6: // 낱장 이미지 6개
		{
			std::vector<Image*> rawImages;
			std::vector<ImageUPtr> imageOwner;

			bool loadFailed = false;
			for (const auto& path : absPaths)
			{
				auto img = Image::Load(path, false);
				if (!img)
				{
					LOG_ERROR("Failed to load cubemap face: {}", path);
					loadFailed = true;
					break;
				}
				rawImages.push_back(img.get());
				imageOwner.push_back(std::move(img));
			}

			if (!loadFailed)
			{
				texture = TextureUtils::LoadCubeMapFromImages(rawImages);
			}
			break;
		}
		default:
			LOG_ERROR("Invalid path count for CubeMap '{}': {}. Expected 1 or 6.", data.key, fileCount);
			success = false;
			continue;
		}

		if (texture)
			RESOURCE.AddResource<CubeTexture>(std::move(texture), data.key, data.paths[0]); // 대표 경로 저장
		else
		{
			LOG_ERROR("Failed to create CubeTexture: '{}'", data.key);
			success = false;
		}
	}
	return success;
}

bool ResourceLoader::LoadModels(const std::vector<ResourceData>& dataList)
{
	bool success = true;
	for (const auto& data : dataList)
	{
		std::string absPath = FILE_MGR.Resolve(data.path);

		// 나중에 여기에 Model Import Settings (meta file) 로직이 추가될 수 있음
		auto model = Model::Load(absPath);

		if (model)
			RESOURCE.AddResource<Model>(std::move(model), data.key, data.path);
		else
		{
			LOG_ERROR("Failed to load Model: '{}' ({})", data.key, absPath);
			success = false;
		}
	}
	return success;
}

bool ResourceLoader::LoadAnimations(const std::vector<ResourceData>& dataList)
{
	bool success = true;
	for (const auto& data : dataList)
	{
		std::string absPath = FILE_MGR.Resolve(data.path);

		// Model이 필요한 경우(Assimp) 처리는 Animation::Load 내부 혹은 별도 로직 필요
		// 현재 구조에서는 단독 Load 호출
		auto anim = Animation::Load(absPath);

		if (anim)
			RESOURCE.AddResource<Animation>(std::move(anim), data.key, data.path);
		else
		{
			LOG_ERROR("Failed to load Animation: '{}'", data.key);
			success = false;
		}
	}
	return success;
}

bool ResourceLoader::LoadBGM(const std::vector<ResourceData>& dataList)
{
	bool success = true;
	for (const auto& data : dataList)
	{
		std::string absPath = FILE_MGR.Resolve(data.path);
		auto bgm = AudioClip::LoadBGM(absPath);

		if (bgm)
			RESOURCE.AddResource<AudioClip>(std::move(bgm), data.key, data.path);
		else
		{
			LOG_ERROR("Failed to load BGM: '{}' ({})", data.key, absPath);
			success = false;
		}
	}
	return success;
}

bool ResourceLoader::LoadSFX(const std::vector<ResourceData>& dataList)
{
	bool success = true;
	for (const auto& data : dataList)
	{
		std::string absPath = FILE_MGR.Resolve(data.path);
		auto sfx = AudioClip::LoadSFX(absPath);

		if (sfx)
			RESOURCE.AddResource<AudioClip>(std::move(sfx), data.key, data.path);
		else
		{
			LOG_ERROR("Failed to load SFX: '{}' ({})", data.key, absPath);
			success = false;
		}
	}
	return success;
}

/*===================================//
//   json menifest parsing methods   //
//===================================*/
std::optional<ResourceManifest> ResourceLoader::ParseManifest(const std::string& manifestPath)
{
	std::ifstream file(manifestPath);
	if (!file.is_open())
	{
		LOG_ERROR("ResourceLoader: Manifest file missing or cannot open: {}", manifestPath);
		return std::nullopt;
	}

	json root;
	try
	{
		file >> root;
	}
	catch (const json::parse_error& e)
	{
		LOG_ERROR("ResourceLoader: JSON Syntax Error in '{}': {}", manifestPath, e.what());
		return std::nullopt;
	}

	ResourceManifest manifest;

	// 1. [스마트 경로 병합] default_root 읽기
	std::string defaultRoot = "";
	if (root.contains("default_root"))
	{
		defaultRoot = root["default_root"].get<std::string>();
	}

	// 2. 파싱 (defaultRoot 전달)
	ParseGeneralResources(root, "bgm", defaultRoot, manifest.bgm);
	ParseGeneralResources(root, "sfx", defaultRoot, manifest.sfx);
	ParseGeneralResources(root, "textures", defaultRoot, manifest.textures);
	ParseGeneralResources(root, "models", defaultRoot, manifest.models);
	ParseGeneralResources(root, "animations", defaultRoot, manifest.animations);

	// 쉐이더는 보통 개별 @Shader 경로를 쓰므로 defaultRoot 적용 여부는 정책 나름
	// 여기서는 명시적인 경로를 선호하므로 defaultRoot를 ""로 넘기거나, 필요한 경우 적용
	ParseShaderResources(root, "shaders", defaultRoot, manifest.shaders);
	ParseGeneralResources(root, "compute_shaders", defaultRoot, manifest.computeShaders);

	ParseCubeMapResources(root, "cubemaps", defaultRoot, manifest.cubemaps);

	return manifest;
}

// ---------------------------------------------------------
// [핵심 변경] 파싱 헬퍼에서 default_root 처리
// ---------------------------------------------------------

void ResourceLoader::ParseGeneralResources(const nlohmann::json& jsonContext, const std::string& categoryKey, const std::string& defaultRoot, std::vector<ResourceData>& outList)
{
	if (!jsonContext.contains(categoryKey)) return;

	const auto& categoryJson = jsonContext[categoryKey];
	if (!categoryJson.is_array()) return;

	for (const auto& item : categoryJson)
	{
		if (!item.contains("key") || !item.contains("path")) continue;

		ResourceData data;
		data.key = item["key"].get<std::string>();

		std::string rawPath = item["path"].get<std::string>();

		// [스마트 병합 로직]
		// 1. 경로가 @로 시작하면 -> 그대로 사용 (Override)
		// 2. 아니면 -> defaultRoot + "/" + rawPath
		if (!rawPath.empty() && rawPath[0] == '@')
		{
			data.path = rawPath;
		}
		else
		{
			// defaultRoot가 비어있지 않으면 슬래시 붙여서 결합
			if (!defaultRoot.empty())
				data.path = defaultRoot + "/" + rawPath;
			else
				data.path = rawPath;
		}

		outList.push_back(data);
	}
}

void ResourceLoader::ParseShaderResources(const nlohmann::json& jsonContext, const std::string& categoryKey, const std::string& defaultRoot, std::vector<ShaderData>& outList)
{
	if (!jsonContext.contains(categoryKey)) return;

	for (const auto& item : jsonContext[categoryKey])
	{
		if (!item.contains("key") || !item.contains("vs") || !item.contains("fs")) continue;

		ShaderData data;
		data.key = item["key"].get<std::string>();

		// VS 파싱
		std::string rawVs = item["vs"].get<std::string>();
		if (!rawVs.empty() && rawVs[0] == '@') data.vsPath = rawVs;
		else data.vsPath = (defaultRoot.empty() ? rawVs : defaultRoot + "/" + rawVs);

		// FS 파싱
		std::string rawFs = item["fs"].get<std::string>();
		if (!rawFs.empty() && rawFs[0] == '@') data.fsPath = rawFs;
		else data.fsPath = (defaultRoot.empty() ? rawFs : defaultRoot + "/" + rawFs);

		outList.push_back(data);
	}
}

void ResourceLoader::ParseCubeMapResources(const nlohmann::json& jsonContext, const std::string& categoryKey, const std::string& defaultRoot, std::vector<CubeMapData>& outList)
{
	if (!jsonContext.contains(categoryKey)) return;

	for (const auto& item : jsonContext[categoryKey])
	{
		if (!item.contains("key") || !item.contains("path")) continue;

		CubeMapData data;
		data.key = item["key"].get<std::string>();

		const auto& pathNode = item["path"];

		// 헬퍼 람다: 경로 결합
		auto ProcessPath = [&](const std::string& raw) -> std::string {
			if (!raw.empty() && raw[0] == '@') return raw;
			return (defaultRoot.empty() ? raw : defaultRoot + "/" + raw);
			};

		if (pathNode.is_string())
		{
			data.paths.push_back(ProcessPath(pathNode.get<std::string>()));
		}
		else if (pathNode.is_array())
		{
			for (const auto& p : pathNode)
			{
				if (p.is_string())
					data.paths.push_back(ProcessPath(p.get<std::string>()));
			}
		}

		outList.push_back(data);
	}
}