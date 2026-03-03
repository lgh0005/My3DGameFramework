#pragma once
#include "Misc/Enums.h"

/*=========================//
//    Resource descriptor  //
//=========================*/
MGF_MGF_CLASS_PTR(ResourceDesc)
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
MGF_CLASS_PTR(AnimationDesc)
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
MGF_CLASS_PTR(AudioClipDesc)
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
MGF_CLASS_PTR(CubeTextureDesc)
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
MGF_CLASS_PTR(ImageDesc)
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
MGF_CLASS_PTR(MaterialDesc)
struct MaterialDesc : public ResourceDesc
{
	virtual ~MaterialDesc();
	MaterialDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	// Material yaml file path
	std::string path;

	// PBR Factors & Paths
	std::string diffuseKey;
	std::string specularKey;
	std::string normalKey;
	std::string heightKey;
	std::string emissionKey;
	std::string aoKey;
	std::string metallicKey;
	std::string roughnessKey;
	std::string ormKey;

	glm::vec4 albedoFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec3 emissiveFactor = { 0.0f, 0.0f, 0.0f };
	float shininess = 32.0f;
	float emissionStrength = 1.0f;
	float heightScale = 1.0f;
	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;

	bool useGlossinessAsRoughness = false;
};

/*=============//
//    Scene    //
//=============*/
MGF_CLASS_PTR(SceneDesc)
struct SceneDesc : public SceneDesc
{
	virtual ~SceneDesc();
	SceneDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
//      Model      //
//================*/
MGF_CLASS_PTR(ModelDesc)
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
MGF_CLASS_PTR(TextureDesc)
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
MGF_CLASS_PTR(FontDesc)
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
MGF_CLASS_PTR(GraphicsProgramDesc)
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
MGF_CLASS_PTR(ComputeProgramDesc)
struct ComputeProgramDesc : public ResourceDesc
{
	virtual ~ComputeProgramDesc();
	ComputeProgramDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
//      Script    //
//================*/
MGF_CLASS_PTR(ScriptDesc)
struct ScriptDesc : public ResourceDesc
{
	virtual ~ScriptDesc();
	ScriptDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
// EnvironmentMap //
//================*/
MGF_CLASS_PTR(EnvironmentMapDesc)
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
MGF_CLASS_PTR(StaticMeshDesc)
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
MGF_CLASS_PTR(SkinnedMeshDesc)
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
MGF_CLASS_PTR(ScreenMeshDesc)
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
MGF_CLASS_PTR(UIMeshDesc)
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
MGF_CLASS_PTR(InstancedMeshDesc)
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
MGF_CLASS_PTR(DebugMeshDesc)
struct DebugMeshDesc : public ResourceDesc
{
	virtual ~DebugMeshDesc() = default;
	DebugMeshDesc(const std::string& filePath = "", const std::string& rscName = "")
		: ResourceDesc(rscName), path(filePath) {
	}
	virtual ResourceType GetDescType() const override { return ResourceType::DebugMesh; }

	std::string path;
};