#pragma once
#include "Resource/IResourceLoader.h"

namespace MGF3D
{
	class UIMeshLoader : public IResourceLoader
	{
	public:
		UIMeshLoader();
		virtual ~UIMeshLoader() override;

	public:
		virtual bool Load
		(
			ResourcePtr& outResource,
			IResourceDescriptorUPtr desc
		) override;
	};
}