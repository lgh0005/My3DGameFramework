#pragma once
#include "Object/Component.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
CLASS_PTR(CubeTexture)
CLASS_PTR(EnvironmentMap)
#pragma endregion

CLASS_PTR(SkyLight)
class SkyLight : public Component
{
	DEFINE_COMPONENT_TYPE(ComponentType::SkyLight)

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
};