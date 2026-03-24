#include "CorePch.h"
#include "IResourceDescriptor.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
	IResourceDescriptor::IResourceDescriptor(const Ptr<MGFType> _type, const MGFName& _name)
		: type(_type), name(_name) { }
	IResourceDescriptor::~IResourceDescriptor() = default;
}