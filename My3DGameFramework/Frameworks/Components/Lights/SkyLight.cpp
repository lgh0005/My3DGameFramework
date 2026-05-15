#include "FrameworkPch.h"
#include "SkyLight.h"
#include "Managers/TypeManager.h"
#include "Mechanics/Lights/SkyCube.h"

namespace MGF3D
{
	SkyLight::SkyLight(ObjectIDHash id, ObjectIDHash ownerID, SkyCubeUPtr skyCube)
		: Super(id, ownerID) ,m_skyCube(std::move(skyCube)) 
	{
		m_skyCube->SetOwner(this);
	}
	SkyLight::~SkyLight() = default;
	SkyLight::SkyLight(SkyLight&& other) noexcept
		: Super(std::move(other)),
		m_skyCube(std::move(other.m_skyCube)),
		m_isMainSkyLight(other.m_isMainSkyLight),
		m_intensity(other.m_intensity),
		m_tint(other.m_tint),
		m_rotation(other.m_rotation),
		m_maxReflectionLod(other.m_maxReflectionLod)
	{
		if (m_skyCube != nullptr) m_skyCube->SetOwner(this);
	}
	SkyLight& SkyLight::operator=(SkyLight&& other) noexcept
	{
		if (this != &other)
		{
			Super::operator=(std::move(other));

			m_skyCube = std::move(other.m_skyCube);
			m_isMainSkyLight = other.m_isMainSkyLight;
			m_intensity = other.m_intensity;
			m_tint = other.m_tint;
			m_rotation = other.m_rotation;
			m_maxReflectionLod = other.m_maxReflectionLod;

			if (m_skyCube != nullptr) m_skyCube->SetOwner(this);
		}
		return *this;
	}

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	int16 SkyLight::s_typeIndex = -1;
	const MGFType* SkyLight::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void SkyLight::SetEnvironmentMap(const EnvironmentMapPtr& envMap)
	{
		if (m_skyCube != nullptr) m_skyCube->SetEnvironmentMap(envMap);
	}

	EnvironmentMapPtr SkyLight::GetEnvironmentMap() const
	{
		if (m_skyCube != nullptr) return m_skyCube->GetEnvironmentMap();
		return nullptr;
	}
}
