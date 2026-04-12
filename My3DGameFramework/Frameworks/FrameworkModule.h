#pragma once
#include "Module/IModuleInterface.h"

namespace MGF3D
{
	class FrameworkModule : public IModuleInterface<FrameworkModule>
	{
		MGF_DECLARE_UTILITIES(FrameworkModule)

	public:
		static void OnRegisterTypes();
		static bool OnInit();
		static bool OnShutdown();
	};
}