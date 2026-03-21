#pragma once
#include "Module/IModuleUnit.h"

namespace MGF3D
{
	class MechanicModule : public IModuleUnit<MechanicModule>
	{
	protected:
		MechanicModule();
		virtual ~MechanicModule();

	public:
		static bool OnModuleInit();
		static void OnShutDown();
	};
}