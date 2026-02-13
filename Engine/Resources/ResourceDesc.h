#pragma once

/*=========================//
//    Resource descriptor  //
//=========================*/
struct ResourceDesc
{
	ResourceDesc(const std::string& rscName = "");
	virtual ~ResourceDesc();
	virtual ResourceType GetDescType() const = 0;

	std::string name;
};

/*================//
//    Animation   //
//================*/
struct AnimationDesc : public ResourceDesc
{
	virtual ~AnimationDesc();
	AnimationDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
//    AudioClip   //
//================*/
struct AudioClipDesc : public ResourceDesc
{
	virtual ~AudioClipDesc();
	AudioClipDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
	AudioType type = AudioType::SFX;
};

/*================//
//    CubeTexture //
//================*/
struct CubeTextureDesc : public ResourceDesc
{
	virtual ~CubeTextureDesc();
	CubeTextureDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
//      Image     //
//================*/
struct ImageDesc : public ResourceDesc
{
	virtual ~ImageDesc();
	ImageDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
	bool isFlipY = true;
};

/*================//
//    Material    //
//================*/
struct MaterialDesc : public ResourceDesc
{
	virtual ~MaterialDesc();
	MaterialDesc(const std::string& filePath, const std::string& rscName = "");
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
//      Model     //
//================*/
struct ModelDesc : public ResourceDesc
{
	virtual ~ModelDesc();
	ModelDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
//     Texture    //
//================*/
struct TextureDesc : public ResourceDesc
{
	virtual ~TextureDesc();
	TextureDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
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

	std::string path;
	uint32 fontSize = 24;
};

/*================//
// GraphicsProgram//
//================*/
struct GraphicsProgramDesc : public ResourceDesc
{
	virtual ~GraphicsProgramDesc();
	GraphicsProgramDesc
	(
		const std::string& vsFile,
		const std::string& fsFile,
		const std::string& rscName = ""
	);
	virtual ResourceType GetDescType() const override;

	std::string vsPath;
	std::string fsPath;
};

/*================//
// ComputeProgram //
//================*/
struct ComputeProgramDesc : public ResourceDesc
{
	virtual ~ComputeProgramDesc();
	ComputeProgramDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
};

/*================//
// EnvironmentMap //
//================*/
struct EnvironmentMapDesc : public ResourceDesc
{
	virtual ~EnvironmentMapDesc();
	EnvironmentMapDesc(const std::string& filePath, const std::string& rscName = "");
	virtual ResourceType GetDescType() const override;

	std::string path;
	bool createIBL = true;
};