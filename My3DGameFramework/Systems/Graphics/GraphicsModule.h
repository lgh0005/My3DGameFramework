#pragma once
#include "Module/IModuleInterface.h"

namespace MGF3D
{
	class GraphicsModule : public IModuleInterface<GraphicsModule>
	{
		MGF_DECLARE_UTILITIES(GraphicsModule)

	public:
		static void OnRegisterTypes();
		static bool OnInit();
		static bool OnShutdown();
	};
}