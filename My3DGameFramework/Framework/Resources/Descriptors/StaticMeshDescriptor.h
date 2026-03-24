#pragma once
#include "Resource/IResourceDescriptor.h"
#include "Vertices/StaticVertex.h"

namespace MGF3D
{
	struct StaticGeometryData
	{
		SVector<StaticVertex> vertices;
		SVector<uint32>       indices;
		uint32                materialIndex { static_cast<uint32>(-1) };
	};

	struct StaticMeshDescriptor : public IResourceDescriptor
	{
		using Super = IResourceDescriptor;

		SVector<StaticGeometryData> geometryList;
		StaticMeshDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name
		);
		virtual ~StaticMeshDescriptor() override;
	};
}