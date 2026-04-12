#pragma once
#include "Module/IModuleInterface.h"

namespace MGF3D
{
	class ApplicationModule : public IModuleInterface<ApplicationModule>
	{
		MGF_DECLARE_UTILITIES(ApplicationModule)

	public:
		static void OnRegisterTypes();
		static bool OnInit();
		static bool OnShutdown();
	};
}