#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(StaticMesh)
#pragma endregion

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