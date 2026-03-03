#pragma once

namespace MGF3D
{
	CLASS_PTR(StaticMesh)

	class GeometryGenerator
	{
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