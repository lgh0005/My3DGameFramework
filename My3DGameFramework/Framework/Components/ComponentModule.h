#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class ComponentModule : public IModuleUnit<ComponentModule>
	{
	protected:
		ComponentModule();
		virtual ~ComponentModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();
	};
}