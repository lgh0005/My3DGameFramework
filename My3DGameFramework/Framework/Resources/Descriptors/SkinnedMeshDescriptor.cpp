#include "ResourcePch.h"
#include "SkinnedMeshDescriptor.h"

namespace MGF3D
{
	SkinnedMeshDescriptor::SkinnedMeshDescriptor(const Ptr<MGFType> _type, const MGFName& _name)
		: Super(_type, _name) { }
	SkinnedMeshDescriptor::~SkinnedMeshDescriptor() = default;
}
