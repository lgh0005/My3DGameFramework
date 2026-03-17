#include "IModuleUnit.h"
#pragma once

namespace MGF3D
{
	template<typename Module> inline IModuleUnit<Module>::IModuleUnit() = default;
	template<typename Module> inline IModuleUnit<Module>::~IModuleUnit() = default;

	template<typename Module>
	inline bool IModuleUnit<Module>::Init()
	{
		return Module::OnModuleInit();
	}

	template<typename Module>
	inline void MGF3D::IModuleUnit<Module>::Shutdown()
	{
		Module::OnShutDown();
	}
}