#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class InitializerModule : public IModuleUnit<InitializerModule>
	{
	protected:
		InitializerModule();
		virtual ~InitializerModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();
	};
}