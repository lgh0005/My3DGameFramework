#include "FrameworkPch.h"
#include "Light.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Light::Light(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) { }
	Light::~Light() = default;

	/*=========================//
	//    Scene Custom Type    //
	//=========================*/
	int16 Light::s_typeIndex = -1;
	const MGFType* Light::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}