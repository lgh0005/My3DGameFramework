#include "EnginePch.h"
#include "ResourceDesc.h"

/*=========================//
//   Resource descriptor   //
//=========================*/
ResourceDesc::ResourceDesc(const std::string& rscName)
	: name(rscName) { }
ResourceDesc::~ResourceDesc() = default;

/*================//
//    Animation   //
//================*/
AnimationDesc::AnimationDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
AnimationDesc::~AnimationDesc() = default;
ResourceType AnimationDesc::GetDescType() const { return ResourceType::Animation; }

/*================//
//    AudioClip   //
//================*/
AudioClipDesc::AudioClipDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
AudioClipDesc::~AudioClipDesc() = default;
ResourceType AudioClipDesc::GetDescType() const { return ResourceType::AudioClip; }

/*================//
//   CubeTexture  //
//================*/
CubeTextureDesc::CubeTextureDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
CubeTextureDesc::~CubeTextureDesc() = default;
ResourceType CubeTextureDesc::GetDescType() const { return ResourceType::CubeTexture; }

/*================//
//      Image     //
//================*/
ImageDesc::ImageDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
ImageDesc::~ImageDesc() = default;
ResourceType ImageDesc::GetDescType() const { return ResourceType::Image; }

/*================//
//    Material    //
//================*/
MaterialDesc::MaterialDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
MaterialDesc::~MaterialDesc() = default;
ResourceType MaterialDesc::GetDescType() const { return ResourceType::Material; }

/*=============//
//    Scene    //
//=============*/
SceneDesc::SceneDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
SceneDesc::~SceneDesc() = default;
ResourceType SceneDesc::GetDescType() const { return ResourceType::Scene; }

/*================//
//      Model     //
//================*/
ModelDesc::ModelDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
ModelDesc::~ModelDesc() = default;
ResourceType ModelDesc::GetDescType() const { return ResourceType::Model; }

/*================//
//     Texture    //
//================*/
TextureDesc::TextureDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
TextureDesc::~TextureDesc() = default;
ResourceType TextureDesc::GetDescType() const { return ResourceType::Texture; }

/*================//
//      Font      //
//================*/
FontDesc::FontDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
FontDesc::~FontDesc() = default;
ResourceType FontDesc::GetDescType() const { return ResourceType::Font; }

/*================//
// GraphicsProgram//
//================*/
GraphicsProgramDesc::GraphicsProgramDesc(const std::string& vsFile, const std::string& fsFile, const std::string& rscName)
	: ResourceDesc(rscName), vsPath(vsFile), fsPath(fsFile) { }
GraphicsProgramDesc::~GraphicsProgramDesc() = default;
ResourceType GraphicsProgramDesc::GetDescType() const { return ResourceType::GraphicsProgram; }

/*================//
// ComputeProgram //
//================*/
ComputeProgramDesc::ComputeProgramDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
ComputeProgramDesc::~ComputeProgramDesc() = default;
ResourceType ComputeProgramDesc::GetDescType() const { return ResourceType::ComputeProgram; }

/*================//
//      Script    //
//================*/
ScriptDesc::ScriptDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
ScriptDesc::~ScriptDesc() = default;
ResourceType ScriptDesc::GetDescType() const { return ResourceType::LuaScript; }

/*================//
// EnvironmentMap //
//================*/
EnvironmentMapDesc::EnvironmentMapDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(rscName), path(filePath) { }
EnvironmentMapDesc::~EnvironmentMapDesc() = default;
ResourceType EnvironmentMapDesc::GetDescType() const { return ResourceType::EnvironmentMap; }