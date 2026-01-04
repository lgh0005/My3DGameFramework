#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
CLASS_PTR(CubeTexture)
CLASS_PTR(EnvironmentMap)
#pragma endregion

CLASS_PTR(SkyLight)
class SkyLight
{
public:
	~SkyLight();
	static SkyLightUPtr Create(const EnvironmentMapPtr& envMap);

public:
	CubeTexture* GetSkybox() const;
	CubeTexture* GetIrradianceMap() const;
	CubeTexture* GetPrefilterMap() const;
	Texture* GetBRDFLookUp() const;

private:
	SkyLight();
	bool Init(const EnvironmentMapPtr& envMap);
	EnvironmentMapPtr m_environmentMap;

	// TODO : 이후에 하늘의 회전(시간 경과에 따른 하늘 회전 등)
	// 등 과 같이 하늘 자체적으로 추가적인 멤버들이 필요하게 될 수도 있음
	// TODO : 이러한 점들을 미뤄볼 때 어쩌면 Component의 역할을 하게 될 수도
	// 있음
};