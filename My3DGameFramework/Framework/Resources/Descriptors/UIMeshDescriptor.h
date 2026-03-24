#pragma once
#include "Resource/IResourceDescriptor.h"
#include "Vertices/UIVertex.h"

namespace MGF3D
{
	struct UIGeometryData
	{
		SVector<UIVertex> vertices;
		SVector<uint32>   indices;
		uint32            materialIndex{ static_cast<uint32>(-1) };
	};

	struct UIMeshDescriptor : public IResourceDescriptor
	{
		using Super = IResourceDescriptor;

		SVector<UIGeometryData> geometryList;
		UIMeshDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name
		);
		virtual ~UIMeshDescriptor() override;
	};
}