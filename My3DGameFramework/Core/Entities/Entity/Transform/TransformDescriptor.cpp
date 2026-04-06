#include "CorePch.h"
#include "TransformDescriptor.h"

namespace MGF3D
{
	TransformDescriptor::TransformDescriptor(Ptr<GameObject> _owner) : owner(_owner) { }
	TransformDescriptor::~TransformDescriptor() = default;
}