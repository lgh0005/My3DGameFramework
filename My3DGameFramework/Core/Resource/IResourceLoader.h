#pragma once
#include "Resource/Resource.h"
#include "Resource/IResourceDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(IResourceLoader)
	class IResourceLoader
	{
	public:
		virtual ~IResourceLoader() = default;
		
		virtual bool Load
		(
			const SharedPtr<Resource>& outResource,
			IResourceDescriptorUPtr desc
		) = 0;
	};
} 