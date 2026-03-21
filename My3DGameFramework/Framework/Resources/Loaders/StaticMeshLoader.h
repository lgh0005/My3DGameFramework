#pragma once
#include "Resource/IResourceLoader.h"

namespace MGF3D
{
	class StaticMeshLoader : public IResourceLoader
	{
	public:
		StaticMeshLoader();
		virtual ~StaticMeshLoader() override;

	public:
		virtual bool Load
		(
			ResourcePtr& outResource,
			IResourceDescriptorUPtr desc
		) override;
	};
}