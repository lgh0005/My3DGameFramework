#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(IResourceDescriptor)
	class IResourceDescriptor
	{
	public:
		IResourceDescriptor(StringHash id);
		virtual ~IResourceDescriptor();

		const StringHash typeID;
		MGFName name;
		MGFPath path;
	};
}