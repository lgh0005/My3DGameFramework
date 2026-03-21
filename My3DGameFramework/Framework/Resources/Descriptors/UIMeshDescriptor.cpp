#include "ResourcePch.h"
#include "UIMeshDescriptor.h"

namespace MGF3D
{
	UIMeshDescriptor::UIMeshDescriptor
	(
		const MGFName& name,
		const MGFPath& path
	) : Super("UIMesh"_sh, name, path) { }
	UIMeshDescriptor::~UIMeshDescriptor() = default;
}