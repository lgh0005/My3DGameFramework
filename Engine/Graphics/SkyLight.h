#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
CLASS_PTR(CubeTexture)
#pragma endregion

CLASS_PTR(SkyLight)
class SkyLight
{
public:
	~SkyLight();
	static SkyLightUPtr Create();
	static SkyLightUPtr CreateStandardSky(const CubeTexturePtr skybox);
	static SkyLightUPtr CreateUniversalSky
	(
		CubeTexturePtr skybox,    CubeTexturePtr irradiance,
		CubeTexturePtr prefilter, TexturePtr	 brdf
	);

public:
	void SetSkybox(CubeTexturePtr texture) { m_skybox = texture; }
	void SetIrradianceMap(CubeTexturePtr texture) { m_irradianceMap = texture; }
	void SetPrefilterMap(CubeTexturePtr texture) { m_prefilterMap = texture; }
	void SetBRDFLookUp(TexturePtr texture) { m_brdfLookUp = texture; }

	CubeTexture* GetSkybox() const { return m_skybox.get(); }
	CubeTexture* GetIrradianceMap() const { return m_irradianceMap.get(); }
	CubeTexture* GetPrefilterMap() const { return m_prefilterMap.get(); }
	Texture* GetBRDFLookUp() const { return m_brdfLookUp.get(); }

private:
	SkyLight();
	CubeTexturePtr	m_skybox			{ nullptr };
	CubeTexturePtr	m_irradianceMap		{ nullptr };
	CubeTexturePtr	m_prefilterMap		{ nullptr };
	TexturePtr		m_brdfLookUp		{ nullptr };

	// TODO : 이후에 하늘의 회전(시간 경과에 따른 하늘 회전 등)
	// 등 과 같이 하늘 자체적으로 추가적인 멤버들이 필요하게 될 수도 있음
};