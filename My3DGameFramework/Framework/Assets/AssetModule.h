#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class AssetModule : public IModuleUnit<AssetModule>
	{
	protected:
		AssetModule();
		virtual ~AssetModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();
	};
}