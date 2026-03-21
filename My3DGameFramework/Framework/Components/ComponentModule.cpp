#include "ComponentPch.h"
#include "ComponentModule.h"

namespace MGF3D
{
	ComponentModule::ComponentModule() = default;
	ComponentModule::~ComponentModule() = default;

	bool ComponentModule::OnModuleInit()
	{
		return false;
	}

	void ComponentModule::OnShutDown()
	{

	}
}