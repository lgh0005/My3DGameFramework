#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(StaticMesh)

	class GeometryUtils
	{
		MGF_DECLARE_UTILITIES(GeometryUtils)

	public:
		static StaticMeshPtr CreateBox();
		static StaticMeshPtr CreatePlane();
		static StaticMeshPtr CreateSphere
		(
			uint32 latiSegmentCount = 16, 
			uint32 longiSegmentCount = 32
		);
	};
}