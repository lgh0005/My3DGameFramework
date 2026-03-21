#include "ResourcePch.h"
#include "SkinnedMeshDescriptor.h"

namespace MGF3D
{
	SkinnedMeshDescriptor::SkinnedMeshDescriptor
	(
		const MGFName& name, 
		const MGFPath& path
	) : Super("SkinnedMesh"_sh, name, path) { }
	SkinnedMeshDescriptor::~SkinnedMeshDescriptor() = default;
}
