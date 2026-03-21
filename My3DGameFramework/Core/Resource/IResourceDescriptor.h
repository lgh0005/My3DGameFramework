#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(IResourceDescriptor)
	struct IResourceDescriptor
	{
		IResourceDescriptor
		(
			StringHash id,
			const MGFName& n, 
			const MGFPath& p
		);
		virtual ~IResourceDescriptor();

		MGFName name;
		MGFPath path;
		const StringHash typeID;
	};
}