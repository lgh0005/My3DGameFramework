#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(IResourceDescriptor)
	class IResourceDescriptor
	{
	public:
		IResourceDescriptor(StringHash id);
		virtual ~IResourceDescriptor();

		MGFName name;
		MGFPath path;
		const StringHash typeID;
	};
}