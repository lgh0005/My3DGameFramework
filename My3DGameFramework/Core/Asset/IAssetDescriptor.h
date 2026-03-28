#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_STRUCT_PTR(IAssetDescriptor)
	struct IAssetDescriptor : public PoolAlloc
	{
		IAssetDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name,
			const MGFPath& _path,
			const SString& _ext
		);
		virtual ~IAssetDescriptor();

		const Ptr<MGFType> type;
		MGFName name;
		MGFPath path;
		SString ext;
	};
}