#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class CoreModule : public IModuleUnit<CoreModule>
	{
	protected:
		CoreModule();
		virtual ~CoreModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();
	};
}