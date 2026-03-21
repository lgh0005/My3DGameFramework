#pragma once
#include "Resource/IResourceLoader.h"

namespace MGF3D
{
	class SkinnedMeshLoader : public IResourceLoader
	{
	public:
		SkinnedMeshLoader();
		virtual ~SkinnedMeshLoader() override;

	public:
		virtual bool Load
		(
			ResourcePtr& outResource,
			IResourceDescriptorUPtr desc
		) override;
	};
}