#include "EnginePch.h"
#include "ResourceDesc.h"

/*=========================//
//   Resource descriptor   //
//=========================*/
ResourceDesc::ResourceDesc(const std::string& filePath, const std::string& rscName)
	: path(filePath), name(rscName) { }
ResourceDesc::~ResourceDesc() = default;

/*================//
//   Animation    //
//================*/
AnimationDesc::AnimationDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
AnimationDesc::~AnimationDesc() = default;
ResourceType AnimationDesc::GetDescType() const { return ResourceType::Animation; }

/*================//
//   AudioClip    //
//================*/
AudioClipDesc::AudioClipDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
AudioClipDesc::~AudioClipDesc() = default;
ResourceType AudioClipDesc::GetDescType() const { return ResourceType::AudioClip; }

/*================//
//   CubeTexture  //
//================*/
CubeTextureDesc::CubeTextureDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
CubeTextureDesc::~CubeTextureDesc() = default;
ResourceType CubeTextureDesc::GetDescType() const { return ResourceType::CubeTexture; }

/*================//
//     Image      //
//================*/
ImageDesc::ImageDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
ImageDesc::~ImageDesc() = default;
ResourceType ImageDesc::GetDescType() const { return ResourceType::Image; }

/*================//
//    Material    //
//================*/
MaterialDesc::MaterialDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
MaterialDesc::~MaterialDesc() = default;
ResourceType MaterialDesc::GetDescType() const { return ResourceType::Material; }

/*================//
//     Model      //
//================*/
ModelDesc::ModelDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
ModelDesc::~ModelDesc() = default;
ResourceType ModelDesc::GetDescType() const { return ResourceType::Model; }

/*================//
//    Texture     //
//================*/
TextureDesc::TextureDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
TextureDesc::~TextureDesc() = default;
ResourceType TextureDesc::GetDescType() const { return ResourceType::Texture; }

/*================//
//      Font      //
//================*/
FontDesc::FontDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
FontDesc::~FontDesc() = default;
ResourceType FontDesc::GetDescType() const { return ResourceType::Font; }

/*================//
//   StaticMesh   //
//================*/
StaticMeshDesc::StaticMeshDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
StaticMeshDesc::~StaticMeshDesc() = default;
ResourceType StaticMeshDesc::GetDescType() const { return ResourceType::StaticMesh; }

/*================//
//  SkinnedMesh   //
//================*/
SkinnedMeshDesc::SkinnedMeshDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
SkinnedMeshDesc::~SkinnedMeshDesc() = default;
ResourceType SkinnedMeshDesc::GetDescType() const { return ResourceType::SkinnedMesh; }

/*================//
//   ScreenMesh   //
//================*/
ScreenMeshDesc::ScreenMeshDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
ScreenMeshDesc::~ScreenMeshDesc() = default;
ResourceType ScreenMeshDesc::GetDescType() const { return ResourceType::ScreenMesh; }

/*================//
// InstancedMesh  //
//================*/
InstancedMeshDesc::InstancedMeshDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
InstancedMeshDesc::~InstancedMeshDesc() = default;
ResourceType InstancedMeshDesc::GetDescType() const { return ResourceType::InstancedMesh; }

/*================//
//     UIMesh     //
//================*/
UIMeshDesc::UIMeshDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
UIMeshDesc::~UIMeshDesc() = default;
ResourceType UIMeshDesc::GetDescType() const { return ResourceType::UIMesh; }

/*================//
//   DebugMesh    //
//================*/
DebugMeshDesc::DebugMeshDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
DebugMeshDesc::~DebugMeshDesc() = default;
ResourceType DebugMeshDesc::GetDescType() const { return ResourceType::DebugMesh; }

/*================//
// GraphicsProgram//
//================*/
GraphicsProgramDesc::GraphicsProgramDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
GraphicsProgramDesc::~GraphicsProgramDesc() = default;
ResourceType GraphicsProgramDesc::GetDescType() const { return ResourceType::GraphicsProgram; }

/*================//
// ComputeProgram //
//================*/
ComputeProgramDesc::ComputeProgramDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
ComputeProgramDesc::~ComputeProgramDesc() = default;
ResourceType ComputeProgramDesc::GetDescType() const { return ResourceType::ComputeProgram; }

/*================//
// EnvironmentMap //
//================*/
EnvironmentMapDesc::EnvironmentMapDesc(const std::string& filePath, const std::string& rscName)
	: ResourceDesc(filePath, rscName) { }
EnvironmentMapDesc::~EnvironmentMapDesc() = default;
ResourceType EnvironmentMapDesc::GetDescType() const { return ResourceType::EnvironmentMap; }