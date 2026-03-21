#pragma once
#include "Resource/IResourceLoader.h"

namespace MGF3D
{
	class ScreenMeshLoader : public IResourceLoader
	{
	public:
		ScreenMeshLoader();
		virtual ~ScreenMeshLoader() override;

	public:
		virtual bool Load
		(
			ResourcePtr& outResource,
			IResourceDescriptorUPtr desc
		) override;
	};
}