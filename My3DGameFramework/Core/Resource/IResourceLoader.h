#pragma once
#include "Resource/Resource.h"
#include "Resource/IResourceDescriptor.h"

namespace MGF3D
{
	class IResourceLoader
	{
	public:
		virtual ~IResourceLoader() = default;
		
		virtual bool Load
		(
			const SharedPtr<Resource>& outResource,
			UniquePtr<const IResourceDescriptor> desc
		) = 0;
	};
} 