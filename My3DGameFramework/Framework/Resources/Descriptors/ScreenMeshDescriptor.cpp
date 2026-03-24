#include "ResourcePch.h"
#include "ScreenMeshDescriptor.h"

namespace MGF3D
{
	ScreenMeshDescriptor::ScreenMeshDescriptor(const Ptr<MGFType> _type, const MGFName& _name)
		: Super(_type, _name) { }
	ScreenMeshDescriptor::~ScreenMeshDescriptor() = default;
}