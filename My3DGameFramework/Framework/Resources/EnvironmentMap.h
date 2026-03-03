#pragma once
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(CubeTexture)
#pragma endregion

CLASS_PTR(EnvironmentMap)
class EnvironmentMap : public Resource
{
	DEFINE_RESOURCE_TYPE(ResourceType::EnvironmentMap, EnvironmentMapDesc)

public:
	virtual ~EnvironmentMap();
	static EnvironmentMapPtr Load(const EnvironmentMapDesc& desc);
	static EnvironmentMapPtr Create(const CubeTexturePtr& cubeTexture);
	static EnvironmentMapPtr CreateIBL(const TexturePtr& hdrTexture);

public:
	virtual EnvironmentMapDesc& GetDesc() override { return m_desc; }
	virtual const ResourceDesc& GetDesc() const override { return m_desc; }

public:
	CubeTexturePtr GetSkybox() const { return m_skybox; }
	CubeTexturePtr GetIrradiance() const { return m_irradiance; }
	CubeTexturePtr GetPrefiltered() const { return m_prefiltered; }
	TexturePtr     GetBrdfLUT() const { return m_brdf; }

private:
	EnvironmentMap();
	EnvironmentMapDesc m_desc;

	CubeTexturePtr m_skybox			{ nullptr };	// 원본(또는 Baked) 스카이박스
	CubeTexturePtr m_irradiance		{ nullptr };	// Diffuse IBL
	CubeTexturePtr m_prefiltered	{ nullptr };	// Specular IBL
	TexturePtr     m_brdf			{ nullptr };	// LUT (Look Up Table)
};