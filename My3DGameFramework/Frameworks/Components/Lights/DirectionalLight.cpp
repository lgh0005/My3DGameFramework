#include "FrameworkPch.h"
#include "DirectionalLight.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	DirectionalLight::DirectionalLight(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) {}
	DirectionalLight::~DirectionalLight() = default;

	/*=========================//
	//    Scene Custom Type    //
	//=========================*/
	int16 DirectionalLight::s_typeIndex = -1;
	const MGFType* DirectionalLight::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}
