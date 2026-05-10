#include "FrameworkPch.h"
#include "SkyLight.h"
#include "Managers/TypeManager.h"
#include "Mechanics/Lights/SkyCube.h"

namespace MGF3D
{
	SkyLight::SkyLight(ObjectIDHash id, ObjectIDHash ownerID, SkyCubeUPtr skyCube)
		: Super(id, ownerID) ,m_skyCube(std::move(skyCube)) { }
	SkyLight::~SkyLight() = default;
	SkyLight::SkyLight(SkyLight&& other) noexcept = default;
	SkyLight& SkyLight::operator=(SkyLight&& other) noexcept = default;

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
