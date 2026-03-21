#include "ResourcePch.h"
#include "ScreenMeshDescriptor.h"

namespace MGF3D
{
	ScreenMeshDescriptor::ScreenMeshDescriptor
	(
		const MGFName& name,
		const MGFPath& path
	) : Super("ScreenMesh"_sh, name, path) { }
	ScreenMeshDescriptor::~ScreenMeshDescriptor() = default;
}