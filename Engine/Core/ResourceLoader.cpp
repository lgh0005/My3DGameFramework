#include "EnginePch.h"
#include "ResourceLoader.h"

#include "Graphics/Geometry.h"
#include "Resources/Meshes/ScreenMesh.h"
#include "Resources/Program.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Model.h"
#include "Resources/Animations/Animation.h"
#include "Resources/Font.h"
#include "Resources/Image.h"
#include "Resources/AudioClip.h"
#include "Resources/Textures/CubeTexture.h"
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
	LOG_INFO("Start Resource Loading...");
	bool success = true;

	// 1. JSON 매니페스트 파싱
	auto manifestOpt = ParseManifest(manifestPath);

	// 2. 파싱 성공 시 외부 리소스 로드 진행
	if (manifestOpt.has_value())
	{
		if (!LoadResourcesFromManifest(manifestOpt.value()))
			success = false;
	}
	else
	{
		// 매니페스트 파일이 없거나 깨짐
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
	bool success = true;

	// 1. 셰이더 로드
	if (!LoadShaders(manifest.shaders)) return false;

	// 3. 텍스쳐 로드
	if (!LoadTextures(manifest.textures)) return false;

	// 4. 큐브 텍스쳐 로드
	if (!LoadCubeTextures(manifest.cubemaps)) return false;

	// 5. 모델 로드
	if (!LoadModels(manifest.models)) return false;

	// 6. 애니메이션 로드
	if (!LoadAnimations(manifest.animations)) return false;

	// 7. 오디오 로드
	if (!LoadBGM(manifest.bgm)) return false;
	if (!LoadSFX(manifest.sfx)) return false;

	return true;
}

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

	// 4. Default Material
	MaterialPtr material;
	
	// 4-1. SRP Material
	material = Material::Create();
	if (!material) return false;
	RESOURCE.AddResource<Material>(material, "material_SRP", "@BuiltIn/Material/SRP");

	// 4-2. URP Material
	material = Material::Create();
	if (!material) return false;
	RESOURCE.AddResource<Material>(material, "material_URP", "@BuiltIn/Material/URP");

	return true;
}

bool ResourceLoader::LoadShaders(const std::vector<ShaderData>& dataList)
{
	bool success = true;
	for (const auto& data : dataList)
	{
		ProgramPtr shader = Program::Create(data.vsPath, data.fsPath);
		if (shader) RESOURCE.AddResource<Program>(std::move(shader), data.key);
		else
		{
			LOG_ERROR("Failed to create Shader: '{}'", data.key);
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
		TexturePtr texture = nullptr;
		std::string path = data.path;

		// 확장자 처리
		std::string ext = fs::path(path).extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		// 확장자에 따른 텍스쳐 로드
		if (ext == ".ktx")
		{
			auto texUPtr = Texture::CreateFromKtxImage(path);
			if (texUPtr) texture = std::move(texUPtr);
		}
		else if (ext == ".hdr")
		{
			auto image = Image::LoadHDR(path, true);
			if (image)
			{
				auto tex = Texture::CreateFromHDR(image.get());
				if (tex) texture = std::move(tex);
			}
		}
		else
		{
			auto image = Image::Load(path, true);
			if (image)
			{
				auto tex = Texture::CreateFromImage(image.get());
				if (tex) texture = std::move(tex);
			}
		}

		// 텍스쳐 등록
		if (texture)
		{
			RESOURCE.AddResource<Texture>(texture, data.key, path);
		}
		else
		{
			LOG_ERROR("Failed to create Texture: '{}' ({})", data.key, path);
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

		// CASE 1: 단일 파일 (KTX, HDR 등)
		if (data.paths.size() == 1)
		{
			std::string path = data.paths[0];
			std::string ext = fs::path(path).extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			if (ext == ".ktx")
			{
				texture = CubeTexture::CreateFromKtxImage(path);
			}
		}

		// CASE 2: 6장 이미지 (Right, Left, Top, Bottom, Front, Back 순서 가정)
		else if (data.paths.size() == 6)
		{
			std::vector<Image*> rawImages;
			std::vector<ImageUPtr> imageOwner; // 메모리 해제 관리용

			bool loadFailed = false;
			for (const auto& path : data.paths)
			{
				// 큐브맵은 Flip하지 않음 (false)
				auto img = Image::Load(path, false);
				if (!img)
				{
					LOG_ERROR("Failed to load cubemap face: {}", path);
					loadFailed = true;
					break;
				}
				rawImages.push_back(img.get());
				imageOwner.push_back(std::move(img)); // 소유권 보존
			}

			if (!loadFailed)
			{
				texture = CubeTexture::CreateFromImages(rawImages);
			}
		}
		else
		{
			LOG_ERROR("Invalid path count for CubeMap '{}': {}. Expected 1 or 6.", data.key, data.paths.size());
			success = false;
			continue;
		}

		// 등록
		if (texture)
		{
			// 경로는 대표로 첫 번째 것을 저장
			RESOURCE.AddResource<CubeTexture>(std::move(texture), data.key, data.paths[0]);
		}
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
		auto model = Model::Load(data.path);
		if (model)
		{
			RESOURCE.AddResource<Model>(std::move(model), data.key, data.path);
		}
		else
		{
			LOG_ERROR("Failed to load Model: '{}' ({})", data.key, data.path);
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
		auto anim = Animation::Load(data.path);

		if (anim)
		{
			RESOURCE.AddResource<Animation>(std::move(anim), data.key, data.path);
		}
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
		auto bgm = AudioClip::LoadBGM(data.path);

		if (bgm)
		{
			RESOURCE.AddResource<AudioClip>(std::move(bgm), data.key, data.path);
		}
		else
		{
			LOG_ERROR("Failed to load BGM: '{}' ({})", data.key, data.path);
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
		auto sfx = AudioClip::LoadSFX(data.path);

		if (sfx)
		{
			RESOURCE.AddResource<AudioClip>(std::move(sfx), data.key, data.path);
		}
		else
		{
			LOG_ERROR("Failed to load SFX: '{}' ({})", data.key, data.path);
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
	// 1. 파일 열기
	std::ifstream file(manifestPath);
	if (!file.is_open())
	{
		LOG_ERROR("ResourceLoader: Manifest file missing or cannot open: {}", manifestPath);
		return std::nullopt;
	}

	// 2. JSON 파싱
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

	// 3. 각 카테고리별 파싱 (키 이름은 JSON 규격과 일치해야 함)
	// 일반 리소스 (Key + Path)
	ParseGeneralResources(root, "bgm", manifest.bgm);
	ParseGeneralResources(root, "sfx", manifest.sfx);
	ParseGeneralResources(root, "textures", manifest.textures); // 복수형
	ParseGeneralResources(root, "models", manifest.models);
	ParseGeneralResources(root, "animations", manifest.animations);

	// 쉐이더 (Key + VS + FS)
	ParseShaderResources(root, "shaders", manifest.shaders);

	// 큐브맵 (Key + Path[Array or String])
	// *헤더에 ParseCubeMapResources 선언이 필요합니다.
	ParseCubeMapResources(root, "cubemaps", manifest.cubemaps); // 복수형

	return manifest;
}

void ResourceLoader::ParseGeneralResources(const nlohmann::json& jsonContext, const std::string& categoryKey, std::vector<ResourceData>& outList)
{
	// 해당 카테고리가 없으면 조용히 리턴 (선택 사항이므로 에러 아님)
	if (!jsonContext.contains(categoryKey)) return;

	const auto& categoryJson = jsonContext[categoryKey];
	if (!categoryJson.is_array())
	{
		LOG_WARN("ResourceLoader: '{}' field exists but is not an array.", categoryKey);
		return;
	}

	for (const auto& item : categoryJson)
	{
		// 필수 키 검사
		if (!item.contains("key") || !item.contains("path"))
		{
			LOG_WARN("ResourceLoader: Skipped invalid item in '{}'. Missing 'key' or 'path'.", categoryKey);
			continue;
		}

		ResourceData data;
		data.key = item["key"].get<std::string>();
		data.path = item["path"].get<std::string>();

		outList.push_back(data);
	}
}

void ResourceLoader::ParseShaderResources(const nlohmann::json& jsonContext, const std::string& categoryKey, std::vector<ShaderData>& outList)
{
	if (!jsonContext.contains(categoryKey)) return;

	const auto& categoryJson = jsonContext[categoryKey];
	if (!categoryJson.is_array()) return;

	for (const auto& item : categoryJson)
	{
		// 쉐이더는 Vertex(vs)와 Fragment(fs) 경로가 필수
		if (!item.contains("key") || !item.contains("vs") || !item.contains("fs"))
		{
			LOG_WARN("ResourceLoader: Skipped invalid shader item. Missing 'key', 'vs', or 'fs'.");
			continue;
		}

		ShaderData data;
		data.key = item["key"].get<std::string>();
		data.vsPath = item["vs"].get<std::string>();
		data.fsPath = item["fs"].get<std::string>();

		outList.push_back(data);
	}
}

void ResourceLoader::ParseCubeMapResources(const nlohmann::json& jsonContext, const std::string& categoryKey, std::vector<CubeMapData>& outList)
{
	if (!jsonContext.contains(categoryKey)) return;

	for (const auto& item : jsonContext[categoryKey])
	{
		if (!item.contains("key") || !item.contains("path")) continue;

		CubeMapData data;
		data.key = item["key"].get<std::string>();

		const auto& pathNode = item["path"];

		// 1. 단일 문자열인 경우 (.ktx, .hdr 등)
		if (pathNode.is_string())
		{
			data.paths.push_back(pathNode.get<std::string>());
		}
		// 2. 배열인 경우 (낱장 이미지 6개)
		else if (pathNode.is_array())
		{
			for (const auto& p : pathNode)
			{
				if (p.is_string()) data.paths.push_back(p.get<std::string>());
			}
		}
		else
		{
			LOG_WARN("ResourceLoader: Invalid path type for cubemap '{}'. Must be string or array.", data.key);
			continue;
		}

		outList.push_back(data);
	}
}

