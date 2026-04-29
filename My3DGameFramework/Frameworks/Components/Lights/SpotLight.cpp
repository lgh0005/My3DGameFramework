#include "FrameworkPch.h"
#include "SpotLight.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	SpotLight::SpotLight(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) { }
	SpotLight::~SpotLight() = default;

	/*=========================//
	//    Scene Custom Type    //
	//=========================*/
	int16 SpotLight::s_typeIndex = -1;
	const MGFType* SpotLight::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void SpotLight::SetSpotAngles(float innerDegree, float outerDegree)
	{
		m_innerCutoff = Math::Cos(Math::ToRadians(innerDegree));
		m_outerCutoff = Math::Cos(Math::ToRadians(outerDegree));
	}
}
