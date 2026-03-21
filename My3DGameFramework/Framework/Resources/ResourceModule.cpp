#include "ResourcePch.h"
#include "ResourceModule.h"

namespace MGF3D
{
	ResourceModule::ResourceModule() = default;
	ResourceModule::~ResourceModule() = default;

	bool ResourceModule::OnModuleInit()
	{
		return false;
	}

	void ResourceModule::OnShutDown()
	{
	}


}