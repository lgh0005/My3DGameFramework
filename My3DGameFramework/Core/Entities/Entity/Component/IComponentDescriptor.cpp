#include "CorePch.h"
#include "IComponentDescriptor.h"

namespace MGF3D
{
	IComponentDescriptor::IComponentDescriptor(const Ptr<MGFType> _type, const MGFName& _name)
		: type(_type), name(_name) { }
	IComponentDescriptor::~IComponentDescriptor() = default;
}