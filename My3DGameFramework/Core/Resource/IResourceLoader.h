#pragma once
#include "Pointer/PoolAlloc.h"
#include "Resource/Resource.h"
#include "Resource/IResourceDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(IResourceLoader)
	class IResourceLoader : public PoolAlloc
	{
	public:
		IResourceLoader();
		virtual ~IResourceLoader();

	public:
		virtual bool Load
		(
			ResourcePtr& outResource,
			IResourceDescriptorUPtr desc
		) = 0;
	};
} 