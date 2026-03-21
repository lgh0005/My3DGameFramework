#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class ResourceModule : public IModuleUnit<ResourceModule>
	{
	protected:
		ResourceModule();
		virtual ~ResourceModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();
	};
}