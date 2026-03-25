#include "CorePch.h"
#include "IAssetImporter.h"
#include "Identity/MGFType.h"

namespace MGF3D
{
	IAssetDescriptor::IAssetDescriptor
	(
		const Ptr<MGFType> _type, 
		const MGFName& _name, 
		const MGFPath& _path,
		const SString& _ext
	)
		: type(_type), 
		  name(_name), 
		  path(_path), 
		  ext(_ext){ }

	IAssetDescriptor::~IAssetDescriptor() = default;
}