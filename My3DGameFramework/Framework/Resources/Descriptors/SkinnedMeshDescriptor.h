#pragma once
#include "Resource/IResourceDescriptor.h"
#include "Vertices/SkinnedVertex.h"

namespace MGF3D
{
	struct SkinnedGeometryData
	{
		SVector<SkinnedVertex> vertices;
		SVector<uint32>        indices;
		uint32                 materialIndex{ static_cast<uint32>(-1) };
	};

	struct SkinnedMeshDescriptor : public IResourceDescriptor
	{
		using Super = IResourceDescriptor;

		SVector<SkinnedGeometryData> geometryList;
		SkinnedMeshDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name
		);
		virtual ~SkinnedMeshDescriptor() override;
	};
}