#pragma once
#include "Misc/Enums.h"

/*=========================//
//    Resource descriptor  //
//=========================*/
CLASS_PTR(ResourceDesc)
struct ResourceDesc
{
	ResourceDesc(const std::string& rscName = "");
	virtual ~ResourceDesc();
	virtual ResourceType GetDescType() const = 0;
	std::string GetName() const { return name; }
	std::string name;
};

/*================//
//    Animation   //
//================*/
CLASS_PTR(AnimationDesc)
struct AnimationDesc : public ResourceDesc
{
	virtual ~AnimationDesc();
	AnimationDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
//    AudioClip   //
//================*/
CLASS_PTR(AudioClipDesc)
struct AudioClipDesc : public ResourceDesc
{
	virtual ~AudioClipDesc();
	AudioClipDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
	AudioType type = AudioType::SFX;
};

/*================//
//    CubeTexture //
//================*/
CLASS_PTR(CubeTextureDesc)
struct CubeTextureDesc : public ResourceDesc
{
	virtual ~CubeTextureDesc();
	CubeTextureDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
//      Image      //
//================*/
CLASS_PTR(ImageDesc)
struct ImageDesc : public ResourceDesc
{
	virtual ~ImageDesc();
	ImageDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
	bool isFlipY = true;
};

/*================//
//    Material    //
//================*/
CLASS_PTR(MaterialDesc)
struct MaterialDesc : public ResourceDesc
{
	virtual ~MaterialDesc();
	MaterialDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;

	// PBR Factors & Paths
	std::string diffusePath;
	std::string specularPath;
	std::string normalPath;
	std::string heightPath;
	std::string emissionPath;
	std::string aoPath;
	std::string metallicPath;
	std::string roughnessPath;
	std::string ormPath;

	glm::vec4 albedoFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec3 emissiveFactor = { 0.0f, 0.0f, 0.0f };
	float shininess = 32.0f;
	float emissionStrength = 1.0f;
	float heightScale = 1.0f;
	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;

	bool useGlossinessAsRoughness = false;
};

/*================//
//      Model      //
//================*/
CLASS_PTR(ModelDesc)
struct ModelDesc : public ResourceDesc
{
	virtual ~ModelDesc();
	ModelDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
//     Texture     //
//================*/
CLASS_PTR(TextureDesc)
struct TextureDesc : public ResourceDesc
{
	virtual ~TextureDesc();
	TextureDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
	bool useMipMap = true;
	bool sRGB = false;
};

/*================//
//      Font       //
//================*/
CLASS_PTR(FontDesc)
struct FontDesc : public ResourceDesc
{
	virtual ~FontDesc();
	FontDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
	uint32 fontSize = 24;
};

/*================//
// GraphicsProgram//
//================*/
CLASS_PTR(GraphicsProgramDesc)
struct GraphicsProgramDesc : public ResourceDesc
{
	virtual ~GraphicsProgramDesc();
	GraphicsProgramDesc
	(
		const std::string& vsFile = "",
		const std::string& fsFile = "",
		const std::string& rscName = ""
	);
	virtual ResourceType GetDescType() const override;

	std::string vsPath;
	std::string tcPath; // Tessellation Control Shader
	std::string tePath; // Tessellation Evaluation Shader
	std::string gsPath; // Geometry Shader
	std::string fsPath;
};

/*================//
// ComputeProgram //
//================*/
CLASS_PTR(ComputeProgramDesc)
struct ComputeProgramDesc : public ResourceDesc
{
	virtual ~ComputeProgramDesc();
	ComputeProgramDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
// EnvironmentMap //
//================*/
CLASS_PTR(EnvironmentMapDesc)
struct EnvironmentMapDesc : public ResourceDesc
{
	virtual ~EnvironmentMapDesc();
	EnvironmentMapDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
//    StaticMesh  //
//================*/
CLASS_PTR(StaticMeshDesc)
struct StaticMeshDesc : public ResourceDesc
{
	virtual ~StaticMeshDesc() = default;
	StaticMeshDesc(const std::string& filePath = "", const std::string& rscName = "")
		: ResourceDesc(rscName), path(filePath) {
	}
	virtual ResourceType GetDescType() const override { return ResourceType::StaticMesh; }

	std::string path;
};

/*================//
//    SkinnedMesh //
//================*/
CLASS_PTR(SkinnedMeshDesc)
struct SkinnedMeshDesc : public ResourceDesc
{
	virtual ~SkinnedMeshDesc() = default;
	SkinnedMeshDesc(const std::string& filePath = "", const std::string& rscName = "")
		: ResourceDesc(rscName), path(filePath) {
	}
	virtual ResourceType GetDescType() const override { return ResourceType::SkinnedMesh; }

	std::string path;
};

/*================//
//    ScreenMesh  //
//================*/
CLASS_PTR(ScreenMeshDesc)
struct ScreenMeshDesc : public ResourceDesc
{
	virtual ~ScreenMeshDesc() = default;
	ScreenMeshDesc(const std::string& filePath = "", const std::string& rscName = "")
		: ResourceDesc(rscName), path(filePath) {
	}
	virtual ResourceType GetDescType() const override { return ResourceType::ScreenMesh; }

	std::string path;
};

/*================//
//      UIMesh     //
//================*/
CLASS_PTR(UIMeshDesc)
struct UIMeshDesc : public ResourceDesc
{
	virtual ~UIMeshDesc() = default;
	UIMeshDesc(const std::string& filePath = "", const std::string& rscName = "")
		: ResourceDesc(rscName), path(filePath) {
	}
	virtual ResourceType GetDescType() const override { return ResourceType::UIMesh; }

	std::string path;
};

/*================//
//   InstancedMesh //
//================*/
CLASS_PTR(InstancedMeshDesc)
struct InstancedMeshDesc : public ResourceDesc
{
	virtual ~InstancedMeshDesc() = default;
	InstancedMeshDesc(const std::string& filePath = "", const std::string& rscName = "")
		: ResourceDesc(rscName), path(filePath) {
	}
	virtual ResourceType GetDescType() const override { return ResourceType::InstancedMesh; }

	std::string path;
};

/*================//
//    DebugMesh    //
//================*/
CLASS_PTR(DebugMeshDesc)
struct DebugMeshDesc : public ResourceDesc
{
	virtual ~DebugMeshDesc() = default;
	DebugMeshDesc(const std::string& filePath = "", const std::string& rscName = "")
		: ResourceDesc(rscName), path(filePath) {
	}
	virtual ResourceType GetDescType() const override { return ResourceType::DebugMesh; }

	std::string path;
};