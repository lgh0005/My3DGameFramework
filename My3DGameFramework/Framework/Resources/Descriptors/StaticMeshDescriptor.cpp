#include "ResourcePch.h"
#include "StaticMeshDescriptor.h"

namespace MGF3D
{
	StaticMeshDescriptor::StaticMeshDescriptor(const Ptr<MGFType> _type, const MGFName& _name)
		: Super(_type, _name) { }
	StaticMeshDescriptor::~StaticMeshDescriptor() = default;
}