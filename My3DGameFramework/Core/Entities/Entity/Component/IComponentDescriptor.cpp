#include "CorePch.h"
#include "IComponentDescriptor.h"
#include "Identity/MGFType.h"
#include "Entities/Entity/GameObject/GameObject.h"

namespace MGF3D
{
	IComponentDescriptor::IComponentDescriptor
	(
		Ptr<GameObject> _owner,
		const Ptr<MGFType> _type, 
		const MGFName& _name
	)
		: owner(_owner), type(_type), name(_name) { }
	IComponentDescriptor::~IComponentDescriptor() = default;
}