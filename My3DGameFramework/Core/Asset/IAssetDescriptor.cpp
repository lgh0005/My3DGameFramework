#include "CorePch.h"
#include "IAssetLoader.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
	IAssetDescriptor::IAssetDescriptor(const Ptr<MGFType> _type, const MGFName& _name, const MGFPath& _path)
		: type(_type), name(_name), path(_path) { }
	IAssetDescriptor::~IAssetDescriptor() = default;
}