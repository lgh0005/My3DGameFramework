#include "AssetPch.h"
#include "AssetModule.h"

namespace MGF3D
{
	AssetModule::AssetModule() = default;
	AssetModule::~AssetModule() = default;

	bool AssetModule::OnModuleInit()
	{
		return false;
	}

	void AssetModule::OnShutDown()
	{

	}
}