#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(StaticMesh)
#pragma endregion

class GeometryGenerator
{
public:
	static StaticMeshUPtr CreateBox();
	static StaticMeshUPtr CreatePlane();
	static StaticMeshUPtr CreateSphere
	(
		uint32 latiSegmentCount = 16,
		uint32 longiSegmentCount = 32
	);
};