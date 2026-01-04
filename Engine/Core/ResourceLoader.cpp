#include "EnginePch.h"
#include "ResourceLoader.h"

#include "Graphics/Geometry.h"
#include "Resources/Program.h"
#include "Resources/Texture.h"
#include "Resources/Model.h"
#include "Resources/Animation.h"
#include "Resources/Font.h"
#include "Resources/Image.h"
#include "Resources/AudioClip.h"
#include "Resources/CubeTexture.h"
#include "Resources/StaticMesh.h"
#include "Resources/Material.h"

#include <nlohmann/json.hpp> 
using json = nlohmann::json;

/*======================//
//  main entry method   //
//======================*/
bool ResourceLoader::Load(const std::string& manifestPath)
{
	LOG_INFO("Start Resource Loading...");
	bool success = true;

	// 1. 빌트인 리소스 로드
	if (!LoadBuiltInResources())
	{
		LOG_ERROR("ResourceLoader: Failed to load built-in resources.");
		success = false;
	}

	// 2. JSON 매니페스트 파싱
	auto manifestOpt = ParseManifest(manifestPath);

	// 3. 파싱 성공 시 외부 리소스 로드 진행
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
	// 1. 셰이더 로드
	if (!LoadShaders(manifest.shaders)) return false;

	// 2. 빌트-인 리소스 로드
	if (!LoadBuiltInResources()) return false;

	// 3. 텍스쳐 로드
	if (!LoadTextures(manifest.textures)) return false;

	// 4. 큐브 텍스쳐 로드
	if (!LoadCubeTextures(manifest.cubeTextures)) return false;

	// 5. 모델 로드
	if (!LoadModels(manifest.models)) return false;

	// 6. 애니메이션 로드
	if (!LoadAnimations(manifest.animations)) return false;

	// 7. 오디오 로드
	if (!LoadAudios(manifest.bgm)) return false;
	if (!LoadAudios(manifest.sfx)) return false;

	return false;
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

	// 4. Default Material
	MaterialPtr material;
	
	// 4-1. SRP Material
	material = Material::Create();
	if (!material) return false;

	// TODO : 값 설정

	RESOURCE.AddResource<Material>(material, "material_SRP", "@BuiltIn/Material/SRP");

	// 4-2. URP Material
	material = Material::Create();
	if (!material) return false;

	// TODO : 값 설정

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
			// TODO : 여기에 break?
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

		// 등록
		if (texture)
		{
			RESOURCE.AddResource(texture, data.key, path);
		}
		else
		{
			LOG_ERROR("Failed to create Texture: '{}' ({})", data.key, path);
			success = false;
		}
	}

	return success;
}

bool ResourceLoader::LoadCubeTextures(const std::vector<ResourceData>& dataList)
{

	return false;
}

bool ResourceLoader::LoadModels(const std::vector<ResourceData>& dataList)
{
	return false;
}

bool ResourceLoader::LoadAnimations(const std::vector<ResourceData>& dataList)
{
	return false;
}

bool ResourceLoader::LoadAudios(const std::vector<ResourceData>& dataList)
{
	return false;
}


