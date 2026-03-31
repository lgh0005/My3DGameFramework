#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_STRUCT_PTR(IComponentDescriptor)
	struct IComponentDescriptor : public PoolAlloc
	{
		using TypeID = StringHash;

		IComponentDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name
		);
		virtual ~IComponentDescriptor();

		const Ptr<MGFType> type;
		MGFName name;
	};
}