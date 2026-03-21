#include "ResourcePch.h"
#include "StaticMeshDescriptor.h"

namespace MGF3D
{
	StaticMeshDescriptor::StaticMeshDescriptor
	(
		const MGFName& name,
		const MGFPath& path
	) : Super("StaticMesh"_sh, name, path) { }
	StaticMeshDescriptor::~StaticMeshDescriptor() = default;
}