#include "CorePch.h"
#include "MGFInputDevice.h"
#include "Managers/TypeManager.h"
#include "Identities/MGFTypeTree.h"
#include <GLFW/glfw3.h>

namespace MGF3D
{
	MGFInputDevice::MGFInputDevice() = default;
	MGFInputDevice::~MGFInputDevice() = default;

	/*================================//
	//    MGFInputDevice Custom Type  //
	//================================*/
	int16 MGFInputDevice::s_typeIndex = -1;
	const MGFType* MGFInputDevice::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Device");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}