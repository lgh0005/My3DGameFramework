#pragma once
#include "Object/Component.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Texture)
	MGF_CLASS_PTR(CubeTexture)
	MGF_CLASS_PTR(EnvironmentMap)

	MGF_CLASS_PTR(SkyLight)
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
}
