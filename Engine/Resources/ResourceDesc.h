#pragma once

/*=========================//
//   Resource descriptor   //
//=========================*/
struct ResourceDesc
{
	ResourceDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ~ResourceDesc();
	virtual ResourceType GetDescType() const = 0;

	std::string path;
	std::string name;
};

/*================//
//   Animation    //
//================*/
struct AnimationDesc : public ResourceDesc
{
	virtual ~AnimationDesc();
	AnimationDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//   AudioClip    //
//================*/
struct AudioClipDesc : public ResourceDesc
{
	virtual ~AudioClipDesc();
	AudioClipDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//   CubeTexture  //
//================*/
struct CubeTextureDesc : public ResourceDesc
{
	virtual ~CubeTextureDesc();
	CubeTextureDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//     Image      //
//================*/
struct ImageDesc : public ResourceDesc
{
	virtual ~ImageDesc();
	ImageDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//    Material    //
//================*/
struct MaterialDesc : public ResourceDesc
{
	virtual ~MaterialDesc();
	MaterialDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//     Model      //
//================*/
struct ModelDesc : public ResourceDesc
{
	virtual ~ModelDesc();
	ModelDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//    Texture     //
//================*/
struct TextureDesc : public ResourceDesc
{
	virtual ~TextureDesc();
	TextureDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	bool isFlipY = true;
	bool useMipMap = true;
	bool sRGB = false;
};

/*================//
//      Font      //
//================*/
struct FontDesc : public ResourceDesc
{
	virtual ~FontDesc();
	FontDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//   StaticMesh   //
//================*/
struct StaticMeshDesc : public ResourceDesc
{
	virtual ~StaticMeshDesc();
	StaticMeshDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//  SkinnedMesh   //
//================*/
struct SkinnedMeshDesc : public ResourceDesc
{
	virtual ~SkinnedMeshDesc();
	SkinnedMeshDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//   ScreenMesh   //
//================*/
struct ScreenMeshDesc : public ResourceDesc
{
	virtual ~ScreenMeshDesc();
	ScreenMeshDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
// InstancedMesh  //
//================*/
struct InstancedMeshDesc : public ResourceDesc
{
	virtual ~InstancedMeshDesc();
	InstancedMeshDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//     UIMesh     //
//================*/
struct UIMeshDesc : public ResourceDesc
{
	virtual ~UIMeshDesc();
	UIMeshDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
//   DebugMesh    //
//================*/
struct DebugMeshDesc : public ResourceDesc
{
	virtual ~DebugMeshDesc();
	DebugMeshDesc(const std::string& filePath = "", const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
// GraphicsProgram//
//================*/
struct GraphicsProgramDesc : public ResourceDesc
{
	virtual ~GraphicsProgramDesc();
	GraphicsProgramDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
// ComputeProgram //
//================*/
struct ComputeProgramDesc : public ResourceDesc
{
	virtual ~ComputeProgramDesc();
	ComputeProgramDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};

/*================//
// EnvironmentMap //
//================*/
struct EnvironmentMapDesc : public ResourceDesc
{
	virtual ~EnvironmentMapDesc();
	EnvironmentMapDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;
};