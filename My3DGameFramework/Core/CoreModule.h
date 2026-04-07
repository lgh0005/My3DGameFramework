#pragma once
#include "Module/IModuleInterface.h"

namespace MGF3D
{
	class CoreModule : public IModuleInterface<CoreModule>
	{
		MGF_DECLARE_UTILITIES(CoreModule)

	public:
		static void OnRegisterTypes();
		static bool OnInit();
		static bool OnShutdown();
	};
}