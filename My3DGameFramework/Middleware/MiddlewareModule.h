#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class MiddlewareModule : public IModuleUnit<MiddlewareModule>
	{
	protected:
		MiddlewareModule();
		virtual ~MiddlewareModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();
	};
}