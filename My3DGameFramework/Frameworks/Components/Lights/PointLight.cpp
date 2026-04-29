#include "FrameworkPch.h"
#include "PointLight.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	PointLight::PointLight(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) { }
	PointLight::~PointLight() = default;

	/*=========================//
	//    Scene Custom Type    //
	//=========================*/
	int16 PointLight::s_typeIndex = -1;
	const MGFType* PointLight::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}
