#include "CorePch.h"
#include "Resource.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Resource::Resource() = default;
	Resource::~Resource() = default;

	/*==============================//
	//       Asset Type             //
	//==============================*/
	int16 Resource::s_typeIndex = -1;
	const MGFType* Resource::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}