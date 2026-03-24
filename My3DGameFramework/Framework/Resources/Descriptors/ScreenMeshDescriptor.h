#pragma once
#include "Resource/IResourceDescriptor.h"
#include "Vertices/ScreenVertex.h"

namespace MGF3D
{
	struct ScreenGeometryData
	{
		SVector<ScreenVertex> vertices;
		SVector<uint32>       indices;
	};

	struct ScreenMeshDescriptor : public IResourceDescriptor
	{
		using Super = IResourceDescriptor;

		SVector<ScreenGeometryData> geometryList;
		ScreenMeshDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name
		);
		virtual ~ScreenMeshDescriptor() override;
	};
}