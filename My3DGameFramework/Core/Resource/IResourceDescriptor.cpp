#include "CorePch.h"
#include "IResourceDescriptor.h"

namespace MGF3D
{
	IResourceDescriptor::IResourceDescriptor
	(
		StringHash id, 
		const MGFName& n, 
		const MGFPath& p
	) { }
	IResourceDescriptor::~IResourceDescriptor() = default;
}