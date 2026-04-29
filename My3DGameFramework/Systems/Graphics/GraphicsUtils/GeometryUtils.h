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
		static StaticMeshPtr CreateCone
		(
			float radius = 1.0f,
			float height = 1.0f,
			uint32 segments = 32
		);

	public:
		template<typename T>
		static void ComputeTangents(Vector<T>& inOutVertices, const Vector<uint32>& indices);
	};
}

#include "GraphicsUtils/GeometryUtils.inl"