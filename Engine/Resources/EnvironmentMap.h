#pragma once
#include "Resources/Resource.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(CubeTexture)
#pragma endregion

CLASS_PTR(EnvironmentMap)
class EnvironmentMap : public Resource
{
public:
	virtual ~EnvironmentMap();
	static const ResourceType s_ResourceType = ResourceType::EnvironmentMap;
	virtual ResourceType GetResourceType() const override { return ResourceType::EnvironmentMap; }
	static EnvironmentMapUPtr Create(const CubeTexturePtr& cubeTexture);
	static EnvironmentMapUPtr CreateIBL(const TexturePtr& hdrTexture);

	CubeTexturePtr GetSkybox() const { return m_skybox; }
	CubeTexturePtr GetIrradiance() const { return m_irradiance; }
	CubeTexturePtr GetPrefiltered() const { return m_prefiltered; }
	TexturePtr     GetBrdfLUT() const { return m_brdf; }

private:
	EnvironmentMap();

	CubeTexturePtr m_skybox			{ nullptr };	// 원본(또는 Baked) 스카이박스
	CubeTexturePtr m_irradiance		{ nullptr };	// Diffuse IBL
	CubeTexturePtr m_prefiltered	{ nullptr };	// Specular IBL
	TexturePtr     m_brdf			{ nullptr };	// LUT (Look Up Table)
};