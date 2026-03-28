#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_STRUCT_PTR(IResourceDescriptor)
	struct IResourceDescriptor : public PoolAlloc
	{
		using TypeID = StringHash;

		IResourceDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name
		);
		virtual ~IResourceDescriptor();

		const Ptr<MGFType> type;
		MGFName name;
	};
}