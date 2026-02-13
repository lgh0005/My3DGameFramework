#pragma once
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(CubeTexture)
#pragma endregion

CLASS_PTR(EnvironmentMap)
class EnvironmentMap : public Resource
{
	DEFINE_RESOURCE_TYPE(ResourceType::EnvironmentMap, EnvironmentMap)

public:
	virtual ~EnvironmentMap();
	static EnvironmentMapPtr Load(const EnvironmentMapDesc& desc);

	CubeTexturePtr GetSkybox() const { return m_skybox; }
	CubeTexturePtr GetIrradiance() const { return m_irradiance; }
	CubeTexturePtr GetPrefiltered() const { return m_prefiltered; }
	TexturePtr     GetBrdfLUT() const { return m_brdf; }

private:
	EnvironmentMap();
	static EnvironmentMapUPtr Create(const CubeTexturePtr& cubeTexture);
	static EnvironmentMapUPtr CreateIBL(const TexturePtr& hdrTexture);

	CubeTexturePtr m_skybox			{ nullptr };	// 원본(또는 Baked) 스카이박스
	CubeTexturePtr m_irradiance		{ nullptr };	// Diffuse IBL
	CubeTexturePtr m_prefiltered	{ nullptr };	// Specular IBL
	TexturePtr     m_brdf			{ nullptr };	// LUT (Look Up Table)
};