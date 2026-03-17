#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class PhysicsModule : public IModuleUnit<PhysicsModule>
	{
	protected:
		PhysicsModule();
		virtual ~PhysicsModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();
	};
}