#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(MGFType)

	MGF_STRUCT_PTR(IAssetDescriptor)
	struct IAssetDescriptor
	{
		IAssetDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name,
			const MGFPath& _path
		);
		virtual ~IAssetDescriptor();

		const Ptr<MGFType> const type;
		MGFName name;
		MGFPath path;
	};
}