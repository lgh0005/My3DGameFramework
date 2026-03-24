#include "ResourcePch.h"
#include "UIMeshDescriptor.h"

namespace MGF3D
{
	UIMeshDescriptor::UIMeshDescriptor(const Ptr<MGFType> _type, const MGFName& _name)
		: Super(_type, _name) { }
	UIMeshDescriptor::~UIMeshDescriptor() = default;
}