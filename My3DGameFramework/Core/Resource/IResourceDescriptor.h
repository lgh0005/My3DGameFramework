#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(MGFType)

	MGF_STRUCT_PTR(IResourceDescriptor)
	struct IResourceDescriptor
	{
		using TypeID = StringHash;

		IResourceDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name
		);
		virtual ~IResourceDescriptor();

		const Ptr<MGFType> const type;
		MGFName name;
	};
}