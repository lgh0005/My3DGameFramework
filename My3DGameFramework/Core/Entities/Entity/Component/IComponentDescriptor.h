#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GameObject)
	MGF_CLASS_PTR(MGFType)

	MGF_STRUCT_PTR(IComponentDescriptor)
	struct IComponentDescriptor : public PoolAlloc
	{
		using TypeID = StringHash;

		IComponentDescriptor
		(
			Ptr<GameObject> _owner,
			const Ptr<MGFType> _type,
			const MGFName& _name
		);
		virtual ~IComponentDescriptor();

		const Ptr<GameObject> owner;
		const Ptr<MGFType> type;
		const MGFName name;
	};
}